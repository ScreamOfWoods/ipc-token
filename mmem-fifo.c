#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>
#include "handlers.h"

#define FIFO "/tmp/token_fifo"

int create_fifo()
{
	int fifo_fd;
	if(mkfifo(FIFO, 0644) == -1) {
		if(errno != EEXIST) {
			fprintf(stderr, "Cannot create FIFO\n");
			handle_error(errno);
		}
	}

	fifo_fd = open(FIFO, O_RDONLY);
	if(fifo_fd == -1) {
		fprintf(stderr, "Cannot open FIFO\n");
		handle_error(errno);
	}

	return fifo_fd;
}

char* read_from_fifo(int fifo_fd)
{
	size_t len;
	char* message;

	if(read(fifo_fd, &len, sizeof(len)) == -1) {
		fprintf(stderr, "Cannot read length to FIFO\n");
		handle_error(errno);
	}
	message = (char*) malloc(len);

	if(read(fifo_fd, message, len) == -1) {
		fprintf(stderr, "Cannot read data from FIFO\n");
		handle_error(errno);
	}

	return message;
}

int main()
{
	int fifo_fd;
	char* message;

	fifo_fd = create_fifo();
	message = read_from_fifo(fifo_fd);

	printf("Reading end of FIFO got message %s\n", message);

	if(unlink(FIFO) == -1) {
		fprintf(stderr, "Cannot delete FIFO\n");
		handle_error(errno);
	}

	free(message);

	return 0;
}
