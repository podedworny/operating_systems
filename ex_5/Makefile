CC = gcc
CFLAGS = -Wall -std=c11 -D_POSIX_C_SOURCE

.PHONY: all clean

all: zad1 sender catcher

zad1: zad1.c
	$(CC) $(CFLAGS) zad1.c -o zad1

sender: sender.c
	$(CC) $(CFLAGS) sender.c -o sender

catcher: catcher.c
	$(CC) $(CFLAGS) catcher.c -o catcher

clean:
	rm -f *.o sender catcher zad1