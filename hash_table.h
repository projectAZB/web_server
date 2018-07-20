//
//  hash_table.h
//  project2
//
//  Created by Adam on 2/3/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#ifndef hash_table_h
#define hash_table_h

#include <stdio.h>

struct hash_table;

typedef struct hash_table hash_table;

typedef hash_table * hash_table_handle;

hash_table_handle create_hash_table(void);
void destroy_hash_table(hash_table_handle this);

void insert(hash_table_handle this, char * key, char * value);
char * search(hash_table_handle this, char * key);

void print_hash_table(hash_table_handle this);

#endif
