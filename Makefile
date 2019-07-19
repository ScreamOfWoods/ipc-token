CC = gcc

all: shmem-mmem pipe-shmem socket-pipe

shmem-mmem: shmem-mmem.c handlers.h
	$(CC) -ggdb $^ -o $@

pipe-shmem: pipe-shmem.c handlers.h pipes.o
	$(CC) -ggdb $^ -o $@

socket-pipe: socket-pipe.c handlers.h pipes.o
	$(CC) $^ -o $@

pipes.o: pipes.c pipes.h
	$(CC) -c $< -o $@

clean:
	rm -rf *.o socket-pipe pipe-shmem shmem-mmem
