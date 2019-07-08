CC = gcc

shmem-mmem: shmem-mmem.c
	$(CC) $^ -o $@

pipe-shmem: pipe-shmem.c
	$(CC) $^ -o $@
