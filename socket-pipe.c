#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include "handlers.h"
#include "pipes.h"

#if 0
char* read_from_pipe()
{
	int full_size = 1024;
	char* message = (char*) malloc(full_size);
	if(message == NULL) {
		handle_error(errno);
	}
	printf("Reading function\n");
	while(1) {
		printf("Loop\n");
		printf("Pipe fds: read %d\n", pipe_fds[0]);
		int res = read(pipe_fds[0], message, full_size);
		printf("Bytes read %d\n", res);
		if(res == -1){
			handle_error(errno);
		}
		printf("Some msg: %s\n", message);
		if(res < full_size) {
			if(message[full_size-1] == '\n')
				break;
			else {
				full_size -= res;
				continue;
			}
		} else {
			printf("Should be read 1024\n");
			break;
		}
	}

	return message;
}
#endif

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
	free(message);
	return 0;
}
