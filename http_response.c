//
//  http_response.c
//  project2
//
//  Created by Adam on 2/3/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include "http_response.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

#include "serving_file_tree.h"
#include "node.h"
#include "response_helper.c"

#define OK "200 OK"
#define MOVED_PERMANENTLY "301 Moved Permanently"
#define NOT_FOUND "404 Not Found"
#define BAD_REQUEST "400 Bad Request"
#define METHOD_NOT_FOUND "405 Method Not Allowed"

#define CONNECTION_CLOSE "Connection: close"
#define LOCATION "Location:"

#define text_html "text/html"
#define text_plain "text/plain"
#define application_pdf "application/pdf"
#define image_png "image/png"
#define image_jpeg "image/jpeg"

struct http_response {
	http_request_handle http_request;
};

http_response_handle create_response_from_request(http_request_handle request)
{
	http_response_handle http_response = (http_response_handle)malloc(sizeof(*http_response));
	http_response->http_request = request;
	return http_response;
}

char * create_request_line(http_response_handle http_response)
{
	hash_table_handle request_hash = get_hash_table(http_response->http_request);
	char * version = search(request_hash, "Version"); //doesn't need to be freed
	char * request_line;
	char hardcode_version[] = "HTTP/1.1";
	if (version == NULL) { //will be NULL in bad request cases
		//need a hardcoded version in case the bad request didn't include the version
		request_line = (char *)malloc(sizeof(char) * (strlen(hardcode_version) + 1));
		strcpy(request_line, hardcode_version);
	}
	else {
		//only use the hash version if there's a valid HTTP/ format used, otherwise it's malformed
		//and needs a hardcoded version
		if (is_valid_http_version(version)) {
			request_line = (char *)malloc(sizeof(char) * (strlen(version) + 1));
			strcpy(request_line, version);
		}
		else { //in case of malformed version, enter it manually
			request_line = (char *)malloc(sizeof(char) * (strlen(hardcode_version) + 1));
			strcpy(request_line, hardcode_version);
		}
	}
	char * method_request = search(request_hash, "Method");
	if (valid_method(method_request))
	{
		if (strcmp(method_request, HTTP_GET) == 0
			|| strcmp(method_request, HTTP_HEAD) == 0)
		{
			serving_file_tree_handle serving_file_tree = create_serving_file_tree("www");
			char * url_request = search(request_hash, "Url");
			node_handle file_node = search_for_file(serving_file_tree, url_request);
			if (file_node) { //file can be served, either redirect or normal
				if (file_node->redirect) { //redirect.defs content
					resize_line(&request_line, MOVED_PERMANENTLY, true, true);
				}
				else { //not a redirect
					if (file_node->type == DT_DIR) { //directory, 404
						resize_line(&request_line, NOT_FOUND, true, true);
					}
					else {
						resize_line(&request_line, OK, true, true);
					}
				}
			}
			else { //file not found
				resize_line(&request_line, NOT_FOUND, true, true);
			}
			destroy_serving_file_tree(serving_file_tree);
		}
		else { //not get or head but valid method; method not found
			resize_line(&request_line, METHOD_NOT_FOUND, true, true);
		}
	}
	else {
		resize_line(&request_line, BAD_REQUEST, true, true);
	}
	
	return request_line;
}

char * create_header_lines(http_response_handle http_response)
{
	hash_table_handle request_hash = get_hash_table(http_response->http_request);
	//always close the connection
	char * header_lines = (char *)malloc(sizeof(char)); //each instance of this terminated by a single \r\n
	memset(header_lines, '\0', 1);
	resize_line(&header_lines, CONNECTION_CLOSE, false, true); //adds Connection: Close and \r\n
	char * method_request = search(request_hash, "Method");
	if (valid_method(method_request) &&
		(strcmp(method_request, HTTP_GET) == 0 || strcmp(method_request, HTTP_HEAD) == 0))
	{
		serving_file_tree_handle serving_file_tree = create_serving_file_tree("www");
		char * url_request = search(request_hash, "Url");
		node_handle file_node = search_for_file(serving_file_tree, url_request);
		if (file_node) { //file can be served, either redirect or normal
			if (file_node->redirect) { //redirect.defs content
				char * location_line = (char *)malloc(sizeof(char) * (strlen(LOCATION) + 1));
				strcpy(location_line, LOCATION);
				resize_line(&location_line, file_node->redirect_url, true, false);
				resize_line(&header_lines, location_line, false, true); //add to header lines and \r\n
				free(location_line);
			}
			else { //not a redirect
				if (file_node->type == DT_DIR) { //file not found
					printf("Directory not servable\n");
				}
				else { //valid file type
					if (is_png(file_node->name)) {
						add_content_type(&header_lines, image_png);
					}
					else if (is_jpeg(file_node->name)) {
						add_content_type(&header_lines, image_jpeg);
					}
					else if (is_pdf(file_node->name)) {
						add_content_type(&header_lines, application_pdf);
					}
					else if (is_html(file_node->name)) {
						add_content_type(&header_lines, text_html);
					}
					else {
						fprintf(stderr, "No matching filetype found for: %s\n", file_node->name);
					}
				}
			}
		}
		
		destroy_serving_file_tree(serving_file_tree);
	}
	
	return header_lines; //ending with a \r\n
}

char * create_reponse_data(http_response_handle http_response)
{
	hash_table_handle request_hash = get_hash_table(http_response->http_request);
	serving_file_tree_handle serving_file_tree = create_serving_file_tree("www");
	char * url_request = search(request_hash, "Url");
	node_handle file_node = search_for_file(serving_file_tree, url_request);
	if (file_node) { //file can be served, either redirect or normal
		if (file_node->redirect) { //redirect.defs content
			return NULL;
		}
		else { //not a redirect
			if (file_node->type == DT_DIR) {
				return NULL;
			}
			else { //valid file type
				if (is_png(file_node->name)) {
					char * png_data = read_bytes(file_node->name);
					destroy_serving_file_tree(serving_file_tree);
					return png_data;
				}
				else if (is_jpeg(file_node->name)) {
					char * jpeg_data = read_bytes(file_node->name);
					destroy_serving_file_tree(serving_file_tree);
					return jpeg_data;
				}
				else if (is_pdf(file_node->name)) {
					char * pdf_data = read_bytes(file_node->name);
					destroy_serving_file_tree(serving_file_tree);
					return pdf_data;
				}
				else if (is_html(file_node->name)) {
					char * html_data = read_file(file_node->name);
					destroy_serving_file_tree(serving_file_tree);
					return html_data;
				}
				else {
					fprintf(stderr, "No matching filetype found for: %s\n", file_node->name);
					destroy_serving_file_tree(serving_file_tree);
					return NULL;
				}
			}
		}
	}
	else { //file not found
		destroy_serving_file_tree(serving_file_tree);
		return NULL;
	}
}

char * get_http_response_data(http_response_handle http_response)
{
	hash_table_handle request_hash = get_hash_table(http_response->http_request);
	char * request_line = create_request_line(http_response);
	char * header_lines = create_header_lines(http_response);
	resize_line(&request_line, header_lines, false, true); //add extra \r\n
	free(header_lines);
	char * method_request = search(request_hash, "Method");
	if (valid_method(method_request) && strcmp(HTTP_GET, method_request) == 0) {
		//add data
		char * response_data = create_reponse_data(http_response);
		if (response_data) {
			resize_line(&request_line, response_data, false, false);
			free(response_data);
		}
	}
	return request_line;
}

void delete_http_reponse(http_response_handle http_response)
{
	free(http_response);
}
