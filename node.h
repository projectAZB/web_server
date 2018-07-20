//
//  node.h
//  project2
//
//  Created by Adam on 2/2/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#ifndef node_h
#define node_h

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

struct node;

typedef struct node node;

typedef node * node_handle;

struct node {
	char * name;
	uint8_t type;
	bool redirect;
	char * redirect_url;
	unsigned int n_c_nodes;
	node_handle * c_nodes; //array of node handles
};

//constructor
node_handle create_node(char * name, uint8_t type, bool redirect, char * redirect_url);
void destroy_node(node_handle node);
void add_c_node(node_handle this, node_handle new);


#endif 
