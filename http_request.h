//
//  http_request.h
//  project2
//
//  Created by Adam on 2/3/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#ifndef http_request_h
#define http_request_h

#include <stdio.h>
#include "hash_table.h"

struct http_request;

typedef struct http_request http_request;

typedef http_request * http_request_handle;

http_request_handle create_http_request(const char * request);
void destroy_http_request(http_request_handle this);

hash_table_handle get_hash_table(http_request_handle http_request);

#endif
