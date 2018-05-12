CC=gcc
CFLAGS= -std=c99 -Wall
%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

all: test clean

test: main.o
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm *.o
