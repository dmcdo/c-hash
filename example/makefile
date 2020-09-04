CC = gcc
CFLAGS = -Wall -pedantic -g

example: chash.o
	$(CC) example.c chash.o -o example $(CFLAGS)

chash.o:
	$(CC) -c ../chash.c -o chash.o $(CFLAGS)

clean:
	rm *.o example
