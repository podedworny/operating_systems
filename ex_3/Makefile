CC = gcc
CFLAGS = -Wall -std=c11

.PHONY: all clean

all: flipper

flipper: flipper.c
	$(CC) $(CFLAGS) flipper.c -o flipper

clean:
	rm -f *.o flipper