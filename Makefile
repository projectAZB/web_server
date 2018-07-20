CC = gcc
CFLAGS = -pedantic -std=gnu99 -Wall
DEBUG_FLAGS = -g $(CFLAGS)

web_server: main.c serving_file_tree.h serving_file_tree.c node.c node.h web_server.c web_server.h\
		hash_table.h hash_table.c hash_node.h hash_node.c http_request.h http_request.c\
		http_response.c http_response.h response_helper.c thpool.c thpool.h
	$(CC) $(DEBUG_FLAGS) http_response.c http_request.c thpool.c hash_node.c hash_table.c serving_file_tree.c node.c web_server.c main.c -o web_server -lpthread

hash_test: hash_test_main.c hash_table.h hash_table.c hash_node.h hash_node.c
	$(CC) $(DEBUG_FLAGS) hash_table.c hash_node.c hash_test_main.c -o hash_test


.PHONY: clean
clean:
	rm -rf *.o *.dSYM web_server hash_test
