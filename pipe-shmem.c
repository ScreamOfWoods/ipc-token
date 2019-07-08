#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <errno.h>

void handle_error(int err_code)
{
	fprintf(stderr, "Got error %s\n", strerror(err_code));
	exit(EXIT_FAILURE);
}

int shared_mem_reader(int shared_mem_id, int shared_segment_size)
{
	char* shared_mem_message;
	struct shmid_ds shared_mem_buffer;

	shared_mem_message = (char*) shmat(shared_mem_id, NULL, 0);

	if(shared_mem_message == (void*) -1) {
		return -1;
	}

	printf("Got message: %s", shared_mem_message);

	if(shmdt(shared_mem_message) == -1) {
		return -1;
	}
	
	if(shmctl(shared_mem_id, IPC_RMID, &shared_mem_buffer) == -1) {
		return -1;
	}
	
	return 0;
}

int main(int argc, char *argv[])
{
	int shared_mem_id = 0;
	const int shared_segment_size = 0x400;

	if(argc < 2) {
		fprintf(stderr, "Usage: ./pipe-shmem shared-memory-id\n");
		exit(EXIT_FAILURE);
	} else {
		shared_mem_id = atoi(argv[1]);
		if(shared_mem_id == 0) {
			fprintf(stderr, "Could not convert %s to number\n", argv[1]);
			exit(EXIT_FAILURE);
		}
	}

	printf("%d\n", shared_mem_id);
	
	if(shared_mem_reader(shared_mem_id, shared_segment_size) == -1) {
		handle_error(errno);
	}

	return 0;
}
