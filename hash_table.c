//
//  hash_table.c
//  project2
//
//  Created by Adam on 2/3/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include "hash_table.h"
#include <stdlib.h>
#include "hash_node.h"
#include <string.h>
#include <assert.h>

#define HT_SIZE 97

struct hash_table {
	hash_node_handle * nodes;
};

hash_table_handle create_hash_table(void)
{
	hash_table_handle hash = (hash_table_handle)malloc(sizeof(*hash));
	hash_node_handle * nodes = (hash_node_handle *)malloc(sizeof(hash_node_handle) * HT_SIZE);
	hash->nodes = nodes;
	/*
	for (int i = 0; i < HT_SIZE; i++)
	{
		printf("%i: %p\n", i, (void *)hash->nodes[i]);
	}
	*/
	memset(hash->nodes, 0x0, HT_SIZE * sizeof(hash_node_handle));
	/*
	for (int i = 0; i < HT_SIZE; i++)
	{
		printf("memset %i: %p\n", i, (void *)hash->nodes[i]);
	}
	*/
	return hash;
}

void r_destroy_hash_nodes(hash_node_handle hash_table)
{
	if (has_next_node(hash_table)) {
		r_destroy_hash_nodes(get_next(hash_table));
	}
	destroy_hash_node(hash_table);
}

void destroy_hash_table(hash_table_handle hash_table)
{
	for (int i = 0; i < HT_SIZE; i++)
	{
		if (hash_table->nodes[i]) {
			r_destroy_hash_nodes(hash_table->nodes[i]);
		}
	}
	free(hash_table->nodes);
	free(hash_table);
}

unsigned int hashed_int(char * key)
{
	unsigned long key_length = strlen(key);
	int x = 1;
	for (int i = 0; i < key_length; i++) {
		int c = (int)key[i];
		x *= c;
	}
	return x;
}

void r_insert(hash_node_handle node, char * key, char * value)
{
	if (has_next_node(node)) {
		r_insert(get_next(node), key, value);
	}
	else { //no next node, set next reference for end node
		hash_node_handle new = create_hash_node(key, value);
		set_next(node, new);
		return;
	}
}

void insert(hash_table_handle hash_table, char * key, char * value)
{
	assert(key != NULL && value != NULL);
	unsigned int index = hashed_int(key) % HT_SIZE;
	hash_node_handle new = hash_table->nodes[index];
	if (new) { //if there's a node recursively insert
		assert(new != NULL);
		r_insert(new, key, value);
	}
	else { //no node at that index (first), insert it
		hash_table->nodes[index] = create_hash_node(key, value);
	}
}

char * r_search(hash_node_handle node, char * key)
{
	assert(node != NULL);
	if (strcmp(get_key(node), key) == 0) {
		return get_value(node);
	}
	else {
		if (has_next_node(node)) {
			return r_search(get_next(node), key);
		}
		else {
			return NULL;
		}
	}
}

char * search(hash_table_handle hash_table, char * key)
{
	int n = hashed_int(key) % HT_SIZE;
	hash_node_handle nth_node = hash_table->nodes[n];
	if (nth_node) {
		assert(nth_node != NULL);
		return r_search(nth_node, key);
	}
	else {
		return NULL;
	}
}

void r_print(hash_node_handle node)
{
	assert(node != NULL);
	printf("key: %s value: %s ", get_key(node), get_value(node));
	if (has_next_node(node)) {
		r_print(get_next(node));
	}
}

void print_hash_table(hash_table_handle hash_table)
{
	for (int i = 0; i < HT_SIZE; i++)
	{
		if (hash_table->nodes[i]) {
			printf("%i:  ", i);
			r_print(hash_table->nodes[i]);
			printf("\n");
		}
		else {
			printf("NULL\n");
		}
	}
}



