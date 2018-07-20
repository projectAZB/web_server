//
//  serving_file_tree.h
//  project2
//
//  Created by Adam on 2/1/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#ifndef serving_file_tree_h
#define serving_file_tree_h

#include <stdio.h>
#include "node.h"

struct serving_file_tree;

typedef struct serving_file_tree serving_file_tree;

typedef serving_file_tree * serving_file_tree_handle;

serving_file_tree_handle create_serving_file_tree(char * directory);
void destroy_serving_file_tree(serving_file_tree_handle this);

node_handle search_for_file(serving_file_tree_handle handle, char * filename); //if NULL, doesn't exist

//for debugging
unsigned int tree_size(serving_file_tree_handle handle);

#endif
