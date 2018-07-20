//
//  http_response.h
//  project2
//
//  Created by Adam on 2/3/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#ifndef http_response_h
#define http_response_h

#include <stdio.h>
#include "http_request.h"

struct http_response;

typedef struct http_response http_response;

typedef http_response * http_response_handle;

http_response_handle create_response_from_request(http_request_handle request);

void delete_http_reponse(http_response_handle http_response);

char * get_http_response_data(http_response_handle http_response);

#endif
