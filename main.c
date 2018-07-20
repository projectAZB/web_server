//
//  main.c
//  project2
//
//  Created by Adam on 2/1/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "web_server.h"

//check if port is valid
bool validPort(const char * input)
{
	int i_value = atoi(input);
	if (i_value > 1024 && i_value < 65336) {
		return true;
	}
	return false;
}

int main(int argc, const char * argv[])
{
	if (argc == 1) {
		fprintf(stderr, "No arguments provided, must enter a port number.");
		exit(1);
	}
	else if (argc > 2) {
		fprintf(stderr, "Too many arguments, only a port number is needed.");
		exit(2);
	}
	//only 1 command-line argument was provided
	const char * port = argv[1];
	if (!validPort(port)) {
		fprintf(stderr, "Port number must be greater than 1024 and less than 65336");
		exit(3);
	}
	
	//create web server
	web_server_handle server_handle = create_web_server(port);
	if (server_handle == NULL) {
		//only way this will fail is if malloc fails (no more memory)
		fprintf(stderr, "Problem creating the web server");
	}
	int status;
	if ((status = start_web_server(server_handle)) != 0) { //if status == 0, server's running
		fprintf(stderr, "Problem starting the web server at port %s", port);
		exit(5);
	}
	destroy_web_server(server_handle);
	
	return 0;
}
