//
//  hash_test_main.c
//  project2
//
//  Created by Adam on 2/3/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "hash_table.h"

#define TEST_SIZE 1000

void test_hash()
{
	char ** keys = (char **)malloc(TEST_SIZE * sizeof(*keys));
	int az_range = 122 - 97 + 1;
	for (int i = 0; i < TEST_SIZE; i++)
	{
		unsigned int word_size = (rand() % 20) + 1; //random size between 1 and 20
		char * word = (char * )malloc(sizeof(char) * (word_size + 1));
		int j;
		for (j = 0; j < word_size; j++)
		{
			int r = rand() % az_range;
			int char_int = 97 + r;
			char c = (char)char_int;
			word[j] = c;
		}
		word[j] = '\0';
		keys[i] = word;
	}
	
	hash_table_handle hash_table = create_hash_table();
	for (int i = 0; i < (TEST_SIZE - 1); i++)
	{
		insert(hash_table, keys[i], keys[i + 1]);
	}
	
	print_hash_table(hash_table);
	
	for (int i = 0; i < (TEST_SIZE - 1); i++)
	{
		char * value = search(hash_table, keys[i]);
		assert(value != NULL);
	}
	for (int i = 0; i < TEST_SIZE; i++)
	{
		free(keys[i]);
	}
	free(keys);
	
	destroy_hash_table(hash_table);
}

int main(int argc, char * argv[])
{
	srand((unsigned int)time(NULL));
	test_hash();
	return 0;
}
