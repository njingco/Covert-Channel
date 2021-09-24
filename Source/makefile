CC=gcc -Wall -g
CLIB = -lpthread -lrt

all: covert_udp 

covert_udp: covert_udp.o 
	$(CC) -o covert_udp covert_udp.o $(CLIB)

clean:
	rm -f *.o covert_udp 
	
covert_udp.o:
	$(CC) -c covert_udp.c