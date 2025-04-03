CC = gcc
CFLAGS = -Wall -std=c11 -D_POSIX_C_SOURCE

.PHONY: all clean

all: main counter  

main: main.c
	$(CC) $(CFLAGS) main.c -o main

counter: counter.c
	$(CC) $(CFLAGS) counter.c -o counter

clean:
	rm -f *.o main counter