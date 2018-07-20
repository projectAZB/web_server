//
//  hash_node.c
//  project2
//
//  Created by Adam on 2/3/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include "hash_node.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct hash_node {
	char * key;
	char * value;
	bool has_next;
	hash_node_handle next;
};

hash_node_handle create_hash_node(char * key, char * value)
{
	hash_node_handle node = (hash_node_handle)malloc(sizeof(*node));
	char * n_key = (char *)malloc(sizeof(char) * (strlen(key) + 1));
	strcpy(n_key, key);
	node->key = n_key;
	char * n_value = (char *)malloc(sizeof(char) * (strlen(value) + 1));
	strcpy(n_value, value);
	node->value = n_value;
	node->has_next = false;
	node->next = NULL;
	return node;
}

void destroy_hash_node(hash_node_handle node)
{
	free(node->key);
	free(node->value);
	free(node);
}

bool has_next_node(hash_node_handle node)
{
	assert(node != NULL);
	return node->has_next;
}

void set_next(hash_node_handle node, hash_node_handle next)
{
	node->has_next = true;
	node->next = next;
}

hash_node_handle get_next(hash_node_handle node)
{
	return node->next;
}

char * get_key(hash_node_handle node)
{
	return node->key;
}

char * get_value(hash_node_handle node)
{
	return node->value;
}

