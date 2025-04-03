CC = gcc
CFLAGS = -Wall -std=c11

.PHONY: all clean

all: client server

server: server.c
	$(CC) $(CFLAGS) server.c -o server

client: client.c
	$(CC) $(CFLAGS) client.c -o client

clean:
	rm -f *.o client server