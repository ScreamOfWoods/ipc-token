#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "handlers.h"
#include "pipes.h"

const char* arg_list[] = {
	"./socket-pipe",
			NULL
};

void spawn()
{
	if(execve("./socket-pipe", arg_list, NULL) == -1) 
			handle_error(errno);
}

void signal_handler()
{
	printf("Got signal_handler\n");
	spawn();
}

void write_to_pipe(char* message, size_t msg_len)
{
	if(write(pipe_fds[1], (void*) message, msg_len) == -1) {
		handle_error(errno);
	}
}

char* shared_mem_reader(int shared_mem_id, int shared_segment_size,
		char* shared_mem_message)
{
	struct shmid_ds shared_mem_buffer;

	shared_mem_message = (char*) shmat(shared_mem_id, NULL, 0);

	if(shared_mem_message == (void*) -1) {
		return NULL;
	}

	printf("Got message: %s", shared_mem_message);
	
	char* returned_msg = (char*) malloc(strlen(shared_mem_message));

	returned_msg = strncpy(returned_msg, shared_mem_message, strlen(shared_mem_message));
	if(shmdt(shared_mem_message) == -1) {
		return NULL;
	}
	
	if(shmctl(shared_mem_id, IPC_RMID, &shared_mem_buffer) == -1) {
		return NULL;
	}
	
	return returned_msg;
}

int main(int argc, char *argv[])
{
	int shared_mem_id = 0;
	char* shared_mem_message = NULL;
	const int shared_segment_size = 0x400;
	pid_t child_pid;
	int len = 0;

	if(pipe(pipe_fds) == -1) {
		handle_error(errno);
	}

	printf("Pipe fds: read %d write %d\n", pipe_fds[0], pipe_fds[1]);

	child_pid = fork();
	if(child_pid == (pid_t) -1) {
		handle_error(errno);
	}

	if(child_pid == (pid_t) 0) {
		struct sigaction caught_sig;
		memset(&caught_sig, 0, sizeof(caught_sig));
		caught_sig.sa_handler = &signal_handler;
		printf("Registering handler for signal!\n");
		if((sigaction(SIGUSR1, &caught_sig, NULL)) == -1) {
			handle_error(errno);
		}
		pause();
	} else {
		close(pipe_fds[0]);
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
		shared_mem_message = shared_mem_reader(shared_mem_id, 
				shared_segment_size, shared_mem_message);
		if(shared_mem_message == NULL) {
			handle_error(errno);
		}

		write_to_pipe(shared_mem_message, len);
		printf("Sending signal SIGUSR!\n");
		kill(child_pid, SIGUSR1);
		close(pipe_fds[1]);
		free(shared_mem_message);
		if((wait(NULL)) == -1) {
			handle_error(errno);
		}
	}

	return 0;
}
