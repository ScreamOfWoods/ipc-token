all: shmem-mmem pipe-shmem socket-pipe fifo-socket mmem-fifo

shmem-mmem: shmem-mmem.c handlers.h
	$(CC) $^ -o $@

pipe-shmem: pipe-shmem.c handlers.h pipes.o
	$(CC) $^ -o $@

socket-pipe: socket-pipe.c handlers.h pipes.o
	$(CC) $^ -o $@

fifo-socket: fifo-socket.c handlers.h
	$(CC) $^ -o $@

mmem-fifo: mmem-fifo.c handlers.h
	$(CC) $^ -o $@

pipes.o: pipes.c pipes.h
	$(CC) -c $< -o $@
	
clean:
	rm -rf *.o socket-pipe pipe-shmem shmem-mmem
