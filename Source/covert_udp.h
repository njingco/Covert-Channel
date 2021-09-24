#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/udp.h>
// #include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/ip.h>

#define VERSION "1.0"

void forgepacket(unsigned int, unsigned int, unsigned short, unsigned short, char *, int, int);
unsigned short in_cksum(unsigned short *, int);
unsigned int host_convert(char *);
void usage(char *);

void client(unsigned int source_addr, unsigned int dest_addr, unsigned short source_port, unsigned short dest_port, char *filename, int ipid);
void server(unsigned int source_addr, char *filename, int ipid);
int charToInt(char msg);
char intToChar(int number);

struct send_udp
{
    struct iphdr ip;
    struct udphdr udp;
} send_udp;

struct recv_udp
{
    struct iphdr ip;
    struct udphdr udp;
    char buffer[10000];
} recv_pkt;

/* From synhose.c by knight */
struct pseudo_header
{
    unsigned int source_address;
    unsigned int dest_address;
    unsigned char placeholder;
    unsigned char protocol;
    unsigned short udp_length;
    struct udphdr udp;
} pseudo_header;