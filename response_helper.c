//
//  response_helper.c
//  project2
//
//  Created by Adam on 2/4/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PNG ".png"
#define JPEG ".jpeg"
#define PDF ".pdf"
#define HTML ".html"

#define CRLF "\r\n"

#define CONTENT_TYPE "Content-Type:"

#define HTTP_GET "GET"
#define HTTP_HEAD "HEAD"
#define HTTP_OPTIONS "OPTIONS"
#define HTTP_POST "POST"
#define HTTP_PUT "PUT"
#define HTTP_CONNECT "CONNECT"
#define HTTP_DELETE "DELETE"

bool valid_method(char * method)
{
	if (method == NULL) return false;
	if (strcmp(HTTP_GET, method) == 0 || strcmp(HTTP_HEAD, method) == 0 || strcmp(HTTP_OPTIONS, method) == 0 ||
		strcmp(HTTP_POST, method) == 0 || strcmp(HTTP_PUT, method) == 0 || strcmp(HTTP_CONNECT, method) == 0 ||
		strcmp(HTTP_DELETE, method) == 0)
	{
		return true;
	}
	return false;
}

bool is_valid_http_version(char * version)
{
	char httpv[] = "HTTP/";
	int http_size = strlen(httpv);
	if (strlen(version) < http_size) {
		return false;
	}
	//version must match http/ in first 5 characters
	if (strncmp(version, httpv, http_size) == 0) {
		return true;
	}
	return false;
}

bool is_png(char * filename)
{
	int png_size = strlen(PNG);
	int filename_size = strlen(filename);
	if (png_size > filename_size) return false;
	bool png_status = true;
	for (int j = (png_size - 1), i = (filename_size - 1); j >=0; i--, j--)
	{
		if (PNG[j] != filename[i]) {
			return false;
		}
	}
	return png_status;
}

bool is_jpeg(char * filename)
{
	int jpeg_size = strlen(JPEG);
	int filename_size = strlen(filename);
	if (jpeg_size > filename_size) return false;
	bool jpeg_status = true;
	for (int j = (jpeg_size - 1), i = (filename_size - 1); j >=0; i--, j--)
	{
		if (JPEG[j] != filename[i]) {
			return false;
		}
	}
	return jpeg_status;
}

bool is_html(char * filename)
{
	int html_size = strlen(HTML);
	int filename_size = strlen(filename);
	if (html_size > filename_size) return false;
	bool html_status = true;
	for (int j = (html_size - 1), i = (filename_size - 1); j >=0; i--, j--)
	{
		if (HTML[j] != filename[i]) {
			return false;
		}
	}
	return html_status;
}

bool is_pdf(char * filename)
{
	int pdf_size = strlen(PDF);
	int filename_size = strlen(filename);
	if (pdf_size > filename_size) return false;
	bool pdf_status = true;
	for (int j = (pdf_size - 1), i = (filename_size - 1); j >=0; i--, j--)
	{
		if (PDF[j] != filename[i]) {
			return false;
		}
	}
	return pdf_status;
}

void resize_line(char ** line, char * new_word, bool space, bool carriage)
{
	int old_length = strlen(*line);
	int line_length = old_length + strlen(new_word) + 1;
	line_length = space ? line_length + 1 : line_length;
	line_length = carriage ? line_length + strlen(CRLF) : line_length;
	*line = (char *)realloc(*line, line_length * sizeof(char));
	if (space) {
		strcat(*line, " ");
	}
	strcat(*line, new_word);
	if (carriage) {
		strcat(*line, CRLF);
	}
}

//reads a text file and returns pointer to the file contents
char * read_file(char * filename)
{
	FILE * input = fopen(filename, "r");
	if (input == NULL || feof(input)) {
		return NULL;
	}
	int buffer_size = 0;
	char * buffer = (char *)malloc(sizeof(char));
	memset(buffer, '\0', 1);
	char previous[100];
	while (fgets(previous, 100, input) != 0)
	{
		int previousSize = strlen(previous) + 1;
		buffer_size += previousSize;
		buffer = (char *)realloc(buffer, buffer_size);
		strcat(buffer, previous);
	}
	
	if (buffer_size == 0) { //EOF was hit immediately, set *line NULL and return -1
		free(buffer);
		return NULL;
	}
	fclose(input);
	
	return buffer;
}

//reads a binary (image/pdf) file and returns pointer to the file contents
char * read_bytes(char * filename)
{
	FILE * input = fopen(filename, "rb");
	if (input == NULL || feof(input)) {
		return NULL;
	}
	int buffer_size = 0;
	char * buffer = (char *)malloc(sizeof(char));
	memset(buffer, '\0', 1);
	char previous[100];
	size_t nread;
	while ((nread = fread(previous, 1, 100, input)) != 0)
	{
		int previousSize = nread;
		buffer_size += previousSize;
		buffer = (char *)realloc(buffer, buffer_size);
		strcat(buffer, previous);
	}
	
	if (buffer_size == 0) {
		free(buffer);
		return NULL;
	}
	fclose(input);
	
	return buffer;
}

void add_content_type(char ** lines, char * content_type)
{
	char * content_type_line = (char *)malloc(sizeof(char) * (strlen(CONTENT_TYPE) + 1));
	strcpy(content_type_line, CONTENT_TYPE);
	resize_line(&content_type_line, content_type, true, false);
	resize_line(lines, content_type_line, false, true);
	free(content_type_line);
}
