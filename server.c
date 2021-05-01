// server code for UDP socket programming
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define IP_PROTOCOL 0
#define PORT 8080
#define SIZE 82

#define sendrecvflag 0
#define nofile "File Not Found!"

int datapacket_num;
int bytes_transmitted;
int packets_retransmitted;
int suc_packets;
int dropped_packets;
int ack_count;
int timeout_count;
bool seq = 0;

bool invoke_seq(){
	seq = !seq;
	return seq;
}

//simulate packet loss by using a random float between 0 and 1.
int sim_loss(double loss)
{
    double simulated = (double) (rand()%100) / 100;
    if(simulated < loss){
        printf("Packet was lost. \n");
        return 0;
    }
    else{
        printf("Packet successfully transferred. \n");
        return 1;
    }
}

//simulate ack loss by using a random float between 0 and 1.
int sim_ack_loss(double loss)
{
    double simulated = (double) (rand()%100) / 100;
    if(simulated < loss){
            printf("Packet was lost. \n");
            return 0;
        }
        else{
            printf("Packet successfully transferred. \n");
            return 1;
        }
}

// function to clear buffer
void clearBuf(char* b)
{
    int i;
    for (i = 0; i < SIZE; i++)
        b[i] = '\0';
}
// function sending file
int sendFile(FILE* fp, char* buf, int s)
{
    int i, len;
    if (fp == NULL) {//if no fp to copy from, nofile = "cannot find file"
	printf("Server located null file.\n");
        strcpy(buf, nofile);
        len = strlen(nofile);
        buf[len] = EOF;
        return 1;
    }
	//fill buffer with data
    int16_t count = 0; //data count
    char ch, ch2;
    for (i = 2; i < s; i++) {
        ch = fgetc(fp);
        buf[i] = ch;
	count++;
        if (ch == EOF)
            return 1;
    }
	//add header in first 2 indices
	buf[0] = count;//each char is 1 byte
	buf[1] = invoke_seq();
    return 0;
}
// driver code
int main()
{
    int sockfd, nBytes;
    struct sockaddr_in addr_con;
    int addrlen = sizeof(addr_con);
    addr_con.sin_family = AF_INET;
    addr_con.sin_port = htons(PORT);
    addr_con.sin_addr.s_addr = INADDR_ANY;
    char net_buf[SIZE];
    FILE* fp;

    // socket()
    sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);

    if (sockfd < 0)
        printf("\nfile descriptor not received!!\n");
    else
        printf("\nfile descriptor %d received\n", sockfd);

    // bind()
    if (bind(sockfd, (struct sockaddr*)&addr_con, sizeof(addr_con)) == 0)
        printf("\nSuccessfully binded!\n");
    else
        printf("\nBinding Failed!\n");

    while (1) {
        printf("\nWaiting for file name...\n");

        // receive file name
        clearBuf(net_buf);

        nBytes = recvfrom(sockfd, net_buf,
                          SIZE, sendrecvflag,
                          (struct sockaddr*)&addr_con, &addrlen);

        fp = fopen(net_buf, "r");
        printf("\nFile Name Received: %s\n", net_buf);
        if (fp == NULL)
            printf("\nFile open failed!\n");
        else
            printf("\nFile Successfully opened!\n");

        while (1) {
            // process
            if (sendFile(fp, net_buf, SIZE)) {
            	//if not simulateloss goes here, and wraps around the sendto
                sendto(sockfd, net_buf, SIZE,
                       sendrecvflag,
                    (struct sockaddr*)&addr_con, addrlen);
                break;
            }

            // send
            //if not simulateloss goes here and wraps around the sendto
            sendto(sockfd, net_buf, SIZE,
                   sendrecvflag,
                (struct sockaddr*)&addr_con, addrlen);
            clearBuf(net_buf);
        }
        if (fp != NULL)
            fclose(fp);
    }
    return 0;
}
