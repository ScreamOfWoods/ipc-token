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
#include <getopt.h>
#include "handlers.h"


void print_usage()
{
	fprintf(stderr, "Usage fifo-socket -s server-ip -p port\n");
	exit(EXIT_FAILURE);
}

char* read_from_server(int client_fd)
{
	int len;
	if(read(client_fd, &len, sizeof(len)) == -1) {
		fprintf(stderr, "Could not read len from socket\n");
		handle_error(errno);
	}
	char *message = (char*) malloc(len);
	if(message == NULL) {
		fprintf(stderr, "Failed to allocate memory for message\n");
		handle_error(errno);
	}
	
	if(read(client_fd, message, len) < len) {
		fprintf(stderr, "Could not read data from socket\n");
		handle_error(errno);
	}

	return message;
}

int main(int argc, char* argv[])
{
	int client_fd;
	int opt;
	int port = 0;
	char server_ip[32];
	struct sockaddr_in address_info;
	
	if(argc < 5) {
		print_usage();
	}

	while((opt = getopt(argc, argv, "s:p:")) != -1) {
		switch(opt) {
			case 's':
				strncpy(server_ip, optarg, 32);
				break;
			case 'p':
				port = atoi(optarg);
				if(!port) {
					fprintf(stderr, "Cannot convert %s to number \n", optarg);
					exit(EXIT_FAILURE);
				}
				break;
			default:
				print_usage();
		}
	}

	if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "Cannot create client socket\n");
		handle_error(errno);
	}

	address_info.sin_family = AF_INET;
	address_info.sin_port = htons(port);

	if(inet_pton(AF_INET, server_ip, &address_info.sin_addr) != 1) {
		fprintf(stderr, "Cannot convert %s to valid ip address\n", server_ip);
		exit(EXIT_FAILURE);
	}

	printf("Trying to connect to server:port %s:%d\n", server_ip, ntohs(address_info.sin_port));
	
	if(connect(client_fd, (struct sockaddr*) &address_info, sizeof(address_info)) == -1) {
		fprintf(stderr, "Failed to initialize connection to server\n");
		handle_error(errno);
	}
	
	char* message = read_from_server(client_fd);
	
	printf("Client socket got message >>%s\n<<", message);

	free(message);

	return 0;
}
