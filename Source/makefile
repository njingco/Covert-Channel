CC=gcc -Wall -g
CLIB = -lpthread -lrt

all: covert 

covert: covert.o 
	$(CC) -o covert covert.o $(CLIB)

clean:
	rm -f *.o core.* covert 
	
covert.o:
	$(CC) -c covert.c