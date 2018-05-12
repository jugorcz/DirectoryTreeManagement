CC=gcc
CFLAGS= -std=c99 -Wall
%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

all: main clean

main: main.o
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm *.o
	
cleanAll:
	rm main
