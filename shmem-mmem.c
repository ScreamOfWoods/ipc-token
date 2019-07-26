#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "handlers.h"

#define SIZE 0x400
#define FIFO "/tmp/token_fifo"
#define MMAP_FILE "/tmp/token_mmap"


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

	if(lseek(mmap_fd, SIZE + 1, SEEK_SET) == -1) {
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
	int segment_id;
	char* shared_memory;
	struct shmid_ds shared_mem_buffer;

	segment_id = shmget(IPC_PRIVATE, SIZE, 
			IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	
	if(segment_id == -1) {
		handle_error(errno);
	}

	shared_memory = (char*) shmat(segment_id, NULL, 0);
	if(shared_memory == (void*) -1) {
		handle_error(errno);
	}

	if(shmctl(segment_id, IPC_STAT,  &shared_mem_buffer) == -1) {
		handle_error(errno);
	}
	
	printf("Shared memory id %d\n", segment_id);
	printf("First step! Enter a string to pass on!\n");

	fgets(shared_memory, shared_mem_buffer.shm_segsz, stdin);
	printf("Alright! Passinig: %s\n", shared_memory);

	if(shmdt(shared_memory) == -1) {
		handle_error(errno);
	}
	
	printf("Waiting for message to get back...\n");
	char* mmap_region = (char*) create_mmap();
	char* message = malloc(SIZE);
	if(message == NULL) {
		fprintf(stderr, "Cannot allocate memory for buffer\n");
		handle_error(errno);
	}

	while(strlen(message) == 0) {
		sscanf(mmap_region, "%s", message);
	}
	printf("Got message back %s\n", message);


	if(munmap(mmap_region, SIZE)) {
		fprintf(stderr, "Failed to unmap mapped memory\n");
	}
	free(message);

	if(unlink(MMAP_FILE) == -1) {
		fprintf(stderr, "Cannot delete FIFO\n");
		handle_error(errno);
	}

	return 0;
}
