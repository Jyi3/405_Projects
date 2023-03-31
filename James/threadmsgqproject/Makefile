
CC = gcc
CFLAGS = -g -Wall

default: threadprog

threadprog: main.c msgq.c zemaphore.c
	$(CC) $(CFLAGS) -o threadprog main.c msgq.c zemaphore.c -lpthread

clean:
	rm -f procprog *.o