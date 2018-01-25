CC=gcc 
CFLAGS=
LDFLAGS= -pthread -lpthread

all: Ass2
program: Ass2.o
mycode.o: Ass2.c 

clean:
	rm -f Ass2 Ass2.o
run: myshell
	./Ass2
