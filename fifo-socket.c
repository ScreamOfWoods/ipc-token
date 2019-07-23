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

	printf("server:port %s:%d\n", server_ip, port);

	return 0;
}
