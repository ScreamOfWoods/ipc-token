#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>
#include "handlers.h"

#define SIZE 0x400
#define FIFO "/tmp/token_fifo"
#define MMAP_FILE "/tmp/token_mmap"

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

void* create_mmap()
{
	int mmap_fd;
	void* mmap_region;

	mmap_fd = open(MMAP_FILE, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	
	if(mmap_fd == -1) {
		if(errno == EEXIST) {
			mmap_fd = open(MMAP_FILE, O_RDWR, S_IRUSR | S_IWUSR);
			if(mmap_fd == -1) {
				fprintf(stderr, "Cannot create file for mapping\n");
				handle_error(errno);
			}
		} 
		else {
			fprintf(stderr, "Cannot create file for mapping\n");
			handle_error(errno);
			
		}
	}

	if(lseek(mmap_fd, SIZE, SEEK_SET) == -1) {
		fprintf(stderr, "Cannot seek to offset %d\n", SIZE);
		handle_error(errno);
	}

	char c = '\0';
	if(write(mmap_fd, &c, 1) == -1) {
		fprintf(stderr, "Cannot set the size of the file\n");
		handle_error(errno);
	}

	if(lseek(mmap_fd, 0, SEEK_SET) == -1) {
		fprintf(stderr, "Cannot seek to the begining of the file\n");
		handle_error(errno);
	}

	mmap_region = mmap(NULL, SIZE, PROT_WRITE | PROT_READ, 
			MAP_SHARED, mmap_fd, 0);
	if(mmap_region == (void*) -1) {
		fprintf(stderr, "Cannot create mapped memory segment\n");
		handle_error(errno);
	}

	return mmap_region;
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
	
	create_mmap();

	free(message);

	return 0;
}
