#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/ip.h>

#define VERSION "1.0"

void forgepacket(unsigned int, unsigned int, unsigned short, unsigned short, char *, int, int, int, int);
unsigned short in_cksum(unsigned short *, int);
unsigned int host_convert(char *);
void usage(char *);
