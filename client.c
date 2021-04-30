// client code for UDP socket programming
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define IP_PROTOCOL 0
#define IP_ADDRESS "127.0.0.1" // localhost
#define PORT 8080
#define SIZE 84

#define sendrecvflag 0

int pack_received;
int dups_received;
int bytes_received;
int good_acks;
int dropped_acks;

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


// function to receive file
int recvFile(char* buf, int s)
{
    int i;
    char ch;
    for (i = 0; i < s; i++) {
        ch = buf[i];
        if (ch == EOF)
            return 1;
	else if (ch == '\0')
	    printf("%s", "/0");
	else
	    printf("%c",ch);
    }       //printf("%c", '\t');//tab between each recv'd packet
	    //printf("\nLast char is %c\n", buf[s]);
    return 0;
}
  
// driver code
int main(){
    int sockfd, nBytes;
    struct sockaddr_in addr_con;
    int addrlen = sizeof(addr_con);
    addr_con.sin_family = AF_INET;
    addr_con.sin_port = htons(PORT);
    addr_con.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    char net_buf[SIZE];
    FILE* fp;
    // socket()
    sockfd = socket(AF_INET, SOCK_DGRAM,
                    IP_PROTOCOL);
  
    if (sockfd < 0)
        printf("\nfile descriptor not received!!\n");
    else
        printf("\nfile descriptor %d received\n", sockfd);
        fp = fopen("out.txt","w"); //create out.txt
        printf("out.txt created.\n");

    while (1) {
        printf("\nPlease enter file name to receive:\n");
        scanf("%s", net_buf);
        sendto(sockfd, net_buf, SIZE,
               sendrecvflag, (struct sockaddr*)&addr_con,
               addrlen);

        printf("\n---------Data Received---------\n");

        while (1) {
            // receive
            bzero(net_buf, SIZE);
            //if not ackloss goes here around recvfrom func
            nBytes = recvfrom(sockfd, net_buf, SIZE,
                              sendrecvflag, (struct sockaddr*)&addr_con,
                              &addrlen);
            // process
            if (recvFile(net_buf, SIZE)) {
            	fprintf(fp, net_buf);
            	fclose(fp);
                break;
            } else {
	        fprintf(fp, net_buf);
        }
     }
        printf("\n-------------------------------\n");
  }
    return 0;
}
