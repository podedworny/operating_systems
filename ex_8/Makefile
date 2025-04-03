CC = gcc
CFLAGS = -Wall -std=c11

.PHONY: all clean

all: user printer


printer: printer.c
	$(CC) $(CFLAGS) printer.c -o printer -lpthread -lrt

user: user.c
	$(CC) $(CFLAGS) user.c -o user -lpthread -lrt

clean:
	rm -f *.o user printer