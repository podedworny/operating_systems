CC = gcc
CFLAGS = -Wall -std=c11

all: server client
	
server: server.c
	$(CC) $(CFLAGS) -o server server.c -lpthread
	
client: client.c
	$(CC) $(CFLAGS) -o client client.c -lpthread

clean:
	rm -f server client
