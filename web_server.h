//
//  web_server.h
//  project2
//
//  Created by Adam on 2/1/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#ifndef web_server_h
#define web_server_h

#include <stdio.h>

struct web_server;

typedef struct web_server web_server;

typedef web_server * web_server_handle;

web_server_handle create_web_server(const char * port);
void destroy_web_server(web_server_handle this);

int start_web_server(web_server_handle server_handle); //starts the web server

#endif 
