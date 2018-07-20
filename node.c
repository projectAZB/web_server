//
//  node.c
//  project2
//
//  Created by Adam on 2/2/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include "node.h"

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/dir.h>

node_handle create_node(char * name, uint8_t type, bool redirect, char * redirect_url)
{
	node_handle node = (node_handle)malloc(sizeof(*node));
	
	unsigned int name_length = strlen(name) + 1;
	char * n_name = (char *)malloc(sizeof(char) * name_length);
	strcpy(n_name, name);
	node->name = n_name;
	node->redirect = redirect;
	
	if (redirect) { //if this is true there will be a redirect_url
		unsigned int redirect_length = strlen(redirect_url) + 1;
		char * r_name = (char *)malloc(sizeof(char) * redirect_length);
		strcpy(r_name, redirect_url);
		node->redirect_url = r_name;
	}
	
	node->n_c_nodes = 0;
	node->c_nodes = (node_handle *)malloc(sizeof(node_handle));
	memset(node->c_nodes, 0x0, sizeof(node_handle));
	node->type = type;
	return node;
}

void add_c_node(node_handle this, node_handle new)
{
	unsigned int new_n_c_nodes = this->n_c_nodes + 1;
	this->c_nodes = (node_handle *)realloc(this->c_nodes, sizeof(node_handle) * new_n_c_nodes);
	this->c_nodes[new_n_c_nodes - 1] = new; //add new node to end of list
	this->n_c_nodes = new_n_c_nodes;
}

void destroy_node(node_handle this)
{
	free(this->name);
	if (this->redirect) {
		free(this->redirect_url);
	}
	free(this->c_nodes);
	free(this);
}
