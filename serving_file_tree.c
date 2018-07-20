//
//  serving_file_tree.c
//  project2
//
//  Created by Adam on 2/1/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include "serving_file_tree.h"

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <stdbool.h>

#define REDIRECT_DIR "redirect.defs"

struct serving_file_tree {
	node_handle root;
	unsigned int size;
};

bool same_or_parent_dir(char * d_name)
{
	if (strcmp(".", d_name) == 0 || strcmp("..", d_name) == 0) {
		return true;
	}
	return false;
}

//bool slash tells whether the forward slash is needed after parent
//caller needs to free
char * get_path(char * parent, char * sub, bool slash)
{
	unsigned int size = strlen(parent) + strlen(sub) + 2; //one for '/' and one for \0
	char * path = (char *)malloc(sizeof(char) * size);
	int i;
	for (i = 0; i < strlen(parent); i++)
	{
		path[i] = parent[i];
	}
	if (slash) {
		path[i++] = '/';
	}
	for (int j = 0; j < strlen(sub); j++, i++)
	{
		path[i] = sub[j];
	}
	path[i] = '\0';
	return path;
}

void file_contents_to_nodes(serving_file_tree_handle tree, char * filename, node_handle parent)
{
	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		perror("Error opening file: ");
	}
	char path[100];
	char url[100];
	while (fscanf(fp, "%s %s", path, url) != EOF) {
		char * full_path = get_path(parent->name, path, false);
		node_handle redirect_node = create_node(full_path, DT_UNKNOWN, true, url);
		//printf("%s\n", full_path);
		free(full_path);
		//add node to parent array
		add_c_node(parent, redirect_node);
		tree->size++;
	}
	fclose(fp);
}

void expand_dir_node(serving_file_tree_handle tree, node_handle dir_node)
{
	DIR * dp = opendir(dir_node->name);
	if (dp ==  NULL) {
		printf("'%s' : ", dir_node->name);
		perror("Error opening the directory");
		exit(100);
	}
	struct dirent * entry;
	while ((entry = readdir(dp))) {
		if (same_or_parent_dir(entry->d_name)) { //if same or parent dir, go to next loop
			continue;
		}
		if (entry->d_type == DT_DIR) {
			char * path = get_path(dir_node->name, entry->d_name, true);
			node_handle new_dir_node = create_node(path, DT_DIR, false, NULL);
			free(path);
			//add to dir_nodes children
			add_c_node(dir_node, new_dir_node);
			tree->size++;
			expand_dir_node(tree, new_dir_node);
		}
		if (entry->d_type == DT_REG) {
			char * path = get_path(dir_node->name, entry->d_name, true);
			if (strcmp(REDIRECT_DIR, entry->d_name) == 0) {
				char * path = get_path(dir_node->name, entry->d_name, true);
				file_contents_to_nodes(tree, path, dir_node);
			}
			else {
				node_handle new_file_node = create_node(path, DT_REG, false, NULL);
				add_c_node(dir_node, new_file_node);
				tree->size++;
			}
			free(path);
		}
	}
}

serving_file_tree_handle create_serving_file_tree(char * directory)
{
	serving_file_tree_handle tree = (serving_file_tree_handle)malloc(sizeof(*tree));
	node_handle root = create_node(directory, DT_DIR, false, NULL);
	tree->root = root;
	tree->size = 1;
	expand_dir_node(tree, tree->root);
	
	return tree;
}

void free_node(node_handle node)
{
	for (int i = 0; i < node->n_c_nodes; i++)
	{
		node_handle c_node = node->c_nodes[i];
		free_node(c_node);
	}
	destroy_node(node);
}

void destroy_serving_file_tree(serving_file_tree_handle this)
{
	free_node(this->root);
}

void r_search_for_file(serving_file_tree_handle tree, node_handle f_node, char * filename, node_handle * result)
{
	if (strcmp(f_node->name, filename) == 0) {
		*result = f_node;
		return;
	}
	else {
		for (int i = 0; i < f_node->n_c_nodes; i++)
		{
			r_search_for_file(tree, f_node->c_nodes[i], filename, result);
		}
		return;
	}
}

node_handle search_for_file(serving_file_tree_handle handle, char * filename)
{
	char * full_path = get_path(handle->root->name, filename, false);
	node_handle found = NULL;
	//kick off search recursion
	r_search_for_file(handle, handle->root, full_path, &found);
	return found;
}

unsigned int tree_size(serving_file_tree_handle this)
{
	return this->size;
}

