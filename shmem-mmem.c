#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <errno.h>
#include "handlers.h"

int main()
{
	int segment_id;
	char* shared_memory;
	struct shmid_ds shared_mem_buffer;
	const int shared_segment_size = 0x400;

	segment_id = shmget(IPC_PRIVATE, shared_segment_size, 
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

	
#if 0
	if(shmctl(segment_id, IPC_RMID, &shared_mem_buffer) == -1) {
		handle_error(errno);
	}
	
	/*
	if(shared_memory != NULL) {
		printf("Successfully deleted!\n");
	}*/
#endif

	return 0;
}
