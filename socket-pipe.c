#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "handlers.h"
#include "pipes.h"

#define PORT 8080

char* read_from_pipe()
{
	int full_size = 1024;
	char* message = (char*) malloc(full_size);
	if(message == NULL) {
		handle_error(errno);
	}
	printf("Read pipe %d\n", pipe_fds[0]);
	int res = read(pipe_fds[0], message, full_size);
	if(res == -1) {
		handle_error(errno);
	}
	if(res <= 1024) {
		printf("%d Bytes read\n", res);
		printf(">>%s<<\n", message);
	}
	return message;
}

void serve(int client_socket, char* message, int length)
{
		if(write(client_socket, &length, sizeof(length)) < sizeof(length)) {
			fprintf(stderr, "Couln't send size of message to client!\n");
			handle_error(errno);
		}
		if(write(client_socket, message, length) < length) {
			fprintf(stderr, "Failed to transmit message to client\n");
			printf("errno %d\n", errno);
			handle_error(errno);
		}

		printf("Send >>%d||%s<< successfully!\n", length, message);
}

void init_server(int* server_fd, struct sockaddr_in* address_info)
{
	int opt = 1;
	(*server_fd) = socket(AF_INET, SOCK_STREAM, 0);

	if((*server_fd) == -1) {
		fprintf(stderr, "Failed to init server!\n");
		handle_error(errno);
	}

	if((setsockopt((*server_fd), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
					&opt, sizeof(opt) )) == -1) {
		fprintf(stderr, "Failed to set options to socket\n");
		handle_error(errno);
	}

	address_info->sin_family = AF_INET;
	address_info->sin_addr.s_addr = INADDR_ANY;
	address_info->sin_port = htons(PORT);

	if(bind((*server_fd), (struct sockaddr*) address_info,
				       	sizeof((*address_info))) == -1) {
		fprintf(stderr, "Error binding socket\n");
		handle_error(errno);
	}
}

void handle_client()
{
	
}

int main(int argc, char* argv[])
{
	pipe_fds[0] = 0;
	if(argc < 2){
		fprintf(stderr, "usage: socket-pipe read-socket-id");
		exit(EXIT_FAILURE);
	} else {
		pipe_fds[0] = atoi(argv[1]);
		if(pipe_fds[0] == 0) {
			fprintf(stderr, "Could not convert %s to number\n", argv[1]);
			exit(EXIT_FAILURE);
		}
		
	}

	char* message = read_from_pipe();
	if(message == NULL) {
		fprintf(stderr, "Nothing read from pipe!\n");
		exit(EXIT_FAILURE);
	}
	
	printf("Got message %s", message);

	close(pipe_fds[0]);

	int server_fd, client_fd;
	int opt = 1;
	struct sockaddr_in address_info;
	int socklen = sizeof(address_info);

	init_server(&server_fd, &address_info);

	if(listen(server_fd, 3) == -1) {
		fprintf(stderr, "Failed to set server socket to listen\n");
	}

	char* server_ip = (char*) malloc(32);
	if(server_ip == NULL) {
		fprintf(stderr, "Cannot allocate memory for server ip\n");
		handle_error(errno);
	}
	inet_ntop(AF_INET, &(address_info.sin_addr),
		       	server_ip, (socklen_t) socklen);

	printf("Server listening on %s:%d\n", server_ip, PORT);

	free(server_ip);

	client_fd = accept(server_fd, (struct sockaddr*) &address_info, 
				(socklen_t*) &socklen);

	if(client_fd == -1){
		fprintf(stderr, "Cannot accept connection\n");
		handle_error(errno);
	}

	serve(client_fd, message, strlen(message));

	free(message);

	return 0;
}
