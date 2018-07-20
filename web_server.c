//
//  web_server.c
//  project2
//
//  Created by Adam on 2/1/18.
//  Copyright © 2018 Adam. All rights reserved.
//

#include "web_server.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#include "thpool.h"

#include "http_request.h"
#include "http_response.h"

#define BUFFER_SIZE 8000

#define BACKLOG 10 // how many pending connections queue will hold

struct web_server {
	char * port_number;
};

web_server_handle create_web_server(const char * port)
{
	web_server_handle server_handle = (web_server_handle)malloc(sizeof(web_server));
	if (server_handle == NULL) {
		return NULL;
	}
	char * port_n = (char *)malloc(sizeof(char) * (strlen(port) + 1));
	strcpy(port_n, port);
	server_handle->port_number = port_n;
	return server_handle;
}

void destroy_web_server(web_server_handle this)
{
	free(this->port_number);
	free(this);
}

//method to get port number
in_port_t getInPort(struct sockaddr * sa)
{
	if (sa->sa_family == AF_INET) { //IPv4
		return ntohs(((struct sockaddr_in *)sa)->sin_port);
	}
	return ntohs(((struct sockaddr_in6 *)sa)->sin6_port);
}

//method to get ip address and return it in 'ip'
void getIP(struct sockaddr * sa, char * ip)
{
	if (sa->sa_family == AF_INET) { //IPv4
		char ip4[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, (struct sockaddr_in *)sa, ip4, INET_ADDRSTRLEN);
		strcpy(ip, ip4);
		return;
	}
	char ip6[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET6, (struct sockaddr_in6 *)sa, ip6, INET6_ADDRSTRLEN);
	strcpy(ip, ip6);
	return;
}

void handle_connection(int socket_fd)
{
	char buffer[BUFFER_SIZE];
	int received = 0;
	received = recv(socket_fd, buffer, BUFFER_SIZE, 0);
	if (received > 0) {
		http_request_handle http_request = create_http_request(buffer);
		memset(buffer, '\0', BUFFER_SIZE);
		http_response_handle http_response = create_response_from_request(http_request);
		char * response_data = get_http_response_data(http_response);
		int status;
		if ((status = send(socket_fd, response_data, strlen(response_data), 0)) == -1) {
			fprintf(stderr, "send error: %s\n", strerror(errno));
		}
	}
	else if (received < 0) { //message was received
		fprintf(stderr, "receive error: %s\n", strerror(errno));
	}
	close(socket_fd);
}

void thread_work(void * arg)
{
	int * socket_fd_p = (int *)(arg);
	int socket_fd = *socket_fd_p;
	handle_connection(socket_fd);
}

int start_web_server(web_server_handle server)
{
	int status;
	struct addrinfo hints, *result;
	int listening_socket_fd; //socket file descriptors
	
	//add hint values for getaddrinfo()
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // IPv4 or IPv6, whichever it chooses
	hints.ai_socktype = SOCK_STREAM; //use TCP connection
	hints.ai_flags = AI_PASSIVE; //fill in the IP for me
	
	//get address info
	if ((status = getaddrinfo(NULL, server->port_number, &hints, &result)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(4);
	}
	
	//make a socket, bind to it, and listen for incoming connections
	if ((listening_socket_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == -1) {
		fprintf(stderr, "socket error: %s\n", strerror(errno));
		exit(5);
	}
	if ((status = bind(listening_socket_fd, result->ai_addr, result->ai_addrlen)) == -1) {
		fprintf(stderr, "bind error: %s\n", strerror(errno));
		exit(6);
	}
	if ((status = listen(listening_socket_fd, BACKLOG)) == -1) {
		fprintf(stderr, "listen error: %s\n", strerror(errno));
		exit(7);
	}
	char ip[INET6_ADDRSTRLEN];
	getIP(result->ai_addr, ip);
	fprintf(stdout, "Listening on port %d at address %s on socket %i\n", getInPort(result->ai_addr), ip, listening_socket_fd);
	
	//initialize the thread pool with 10 threads
	threadpool thpool = thpool_init(10);
	while (true)
	{
		int new_socket_fd;
		struct sockaddr_storage their_addr;
		socklen_t addr_size = 0;
		addr_size = sizeof(their_addr);
		if ((new_socket_fd = accept(listening_socket_fd, (struct sockaddr *)&their_addr, &addr_size)) == -1) {
			fprintf(stderr, "accept error %s\n", strerror(errno));
			exit(8);
		}
		else { //handle connection
			//use third party api to manage the thread pool
			thpool_add_work(thpool, thread_work, (void *)&new_socket_fd);
		}
	}
	thpool_destroy(thpool);
	
	freeaddrinfo(result);
	
	return 0;
}
