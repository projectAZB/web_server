//
//  hash_node.h
//  project2
//
//  Created by Adam on 2/3/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#ifndef hash_node_h
#define hash_node_h

#include <stdio.h>
#include <stdbool.h>

struct hash_node;

typedef struct hash_node hash_node;

typedef hash_node * hash_node_handle;

hash_node_handle create_hash_node(char * key, char * value);
void destroy_hash_node(hash_node_handle this);

bool has_next_node(hash_node_handle this);
void set_next(hash_node_handle this, hash_node_handle next);
hash_node_handle get_next(hash_node_handle this);

char * get_key(hash_node_handle this);
char * get_value(hash_node_handle this);

#endif
