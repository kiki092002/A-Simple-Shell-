#the compiler to use
CC=g++
CFLAGS=-Wall -c -Werror
all: myshell
myshell: myshell.o
	$(CC) myshell.o -o myshell
myshell.o: myshell.cpp
	$(CC) $(CFLAGS) myshell.cpp
clean:
	rm -f *o myshell

