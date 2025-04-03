CC = gcc
CFLAGS = -Wall -std=c11 -O0

.PHONY: clean all 

all: collatz_a collatz_s collatz_d

collatz_a_o: collatz.c
	$(CC) $(CFLAGS) -c -o collatz_a.o collatz.c

lib_collatz_a:
	make collatz_a_o
	ar rcs libcollatz.a collatz_a.o

collatz_so_o: collatz.c
	$(CC) $(CFLAGS) -fPIC -c -o collatz_s.o collatz.c

lib_collatz_so: 
	make collatz_so_o
	$(CC) $(CFLAGS) -shared -fPIC -o libcollatz.so collatz_s.o

collatz_a: main.c 
	make lib_collatz_a
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) main.o libcollatz.a -o main_a

collatz_s: main.c 
	make lib_collatz_so
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) -o main_s main.o -L. -lcollatz -Wl,-rpath=.

collatz_d: main.c
	make lib_collatz_so
	$(CC) $(CFLAGS) -D DYNAMIC -c main.c
	$(CC) $(CFLAGS) -o main_d main.o
	
clean:
	rm -f *.o main_a main_s main_d *.a *.so
