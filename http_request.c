//
//  http_request.c
//  project2
//
//  Created by Adam on 2/3/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include "http_request.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

struct http_request {
	hash_table_handle hash_table;
};

hash_table_handle get_hash_table(http_request_handle http_request)
{
	return http_request->hash_table;
}

void process_request_line(http_request_handle http_request, const char * request, int * i)
{
	unsigned int req_length = strlen(request);
	bool req_line_processed = false;
	bool method_processed = false;
	bool url_processed = false;
	for (; (*i) < req_length; (*i)++)
	{
		if (!req_line_processed) {
			if (method_processed && url_processed) { //process version next
				char version[100];
				for (int j = 0; j < 100; j++, (*i)++)
				{
					if (request[*i] == '\r') {
						version[j] = '\0';
						insert(http_request->hash_table, "Version", version);
						req_line_processed = true;
						break;
					}
					else {
						version[j] = request[*i];
						continue;
					}
				}
			}
			else if (method_processed) { //process url next
				char url[100];
				for (int j = 0; j < 100; j++, (*i)++)
				{
					if (request[*i] == ' ') {
						url[j] = '\0';
						insert(http_request->hash_table, "Url", url);
						url_processed = true;
						break;
					}
					else {
						url[j] = request[*i];
						continue;
					}
				}
			}
			else { //process method
				char method[40];
				for (int j = 0; j < 40; j++, (*i)++)
				{
					if (request[*i] == ' ') {
						method[j] = '\0';
						insert(http_request->hash_table, "Method", method);
						method_processed = true;
						break;
					}
					else {
						method[j] = request[*i];
						continue;
					}
				}
			}
		}
		else {
			if (request[*i] == '\n') {
				(*i)++;
				break;
			}
		}
	}
}

void process_head_lines(http_request_handle http_request, const char * request, int * i)
{
	unsigned int req_length = strlen(request);
	for (; *i < req_length; (*i)++)
	{
		if (request[*i] == '\r' && request[*i + 1] == '\n') {
			//header lines have ended, break out of loop and end processing
			break;
		}
		char key[60];
		//get the key
		for (int j = 0; j < 60; j++, (*i)++)
		{
			if (request[*i] == ':' && request[*i + 1] == ' ') {
				key[j] = '\0';
				(*i)++; //increment to space
				(*i)++; //increment to first char after space
				break; //break out of loop to value processing
			}
			else {
				key[j] = request[*i];
				continue;
			}
		}
		char value[60];
		//get the value
		for (int j = 0; j < 60; j++, (*i)++)
		{
			if (request[*i] == '\r' && request[*i + 1] == '\n') { //end of header line
				value[j] = '\0';
				(*i)++; //increment to newline
				break; //next line of outer loop will inc to next line
			}
			else {
				value[j] = request[*i];
				continue;
			}
		}
		insert(http_request->hash_table, key, value);
	}
}

http_request_handle create_http_request(const char * request)
{
	http_request_handle http_request = (http_request_handle)malloc(sizeof(*http_request));
	hash_table_handle hash_table = create_hash_table();
	http_request->hash_table = hash_table;
	//printf(request);
	int i = 0; //tracking index in request string
	process_request_line(http_request, request, &i);
	process_head_lines(http_request, request, &i);
	//print_hash_table(http_request->hash_table);
	
	return http_request;
}

void destroy_http_request(http_request_handle this)
{
	destroy_hash_table(this->hash_table);
	free(this);
}
