CC = gcc
CFLAGS = -Wall -std=c11 -g

.PHONY: clean all

all: countdown

countdown: countdown.c
	$(CC) $(CFLAGS) countdown.c -o countdown 

clean:
	rm -f *.o countdown
