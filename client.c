// client code for UDP socket programming
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#define IP_PROTOCOL 0
#define IP_ADDRESS "127.0.0.1" // localhost
#define PORT 8080
#define SIZE 82

#define sendrecvflag 0

int pack_received;
int dups_received;
int bytes_received;
int good_acks;
int dropped_acks;
bool seq;

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

//function to strip header information
char* strip_header(char* buffer){
	for(int i=0;i<SIZE-1;i++){
	    buffer[i] = buffer[i+2];}
	buffer[SIZE-1] = '\0';
	buffer[SIZE] = '\0';
    return buffer;
}

char* add_header(char* buffer){ //only used once ;-; cannot be used in bytestream
	int count = 0; //keeps track of number of bytes
	for(int i=SIZE; i>1; i--){ //right to left
	    if(buffer[i] != '\0'){count++;}
	    buffer[i] = buffer[i-2]; //shift right twice
	}
	buffer[0] = count;
	buffer[1] = seq;
    return buffer;
}

void update_legacy(char* leg_buf, char* new_buf){
	leg_buf[0] = new_buf[0];
	leg_buf[1] = new_buf[1];
}
void check_buf(char* buf){
	printf("CHECKING BUFFER:%c", '\n');
	for(int i=0;i<SIZE;i++){
	    printf("%c",buf[i]);
	}
	printf("\n CHECK \n");
}

// function to receive file
int recvFile(char* buf, int s)
{
    int i;
    char ch;
	printf("\nPacket size: %d\t, seq: %d\n",buf[0], buf[1]);
//	buf[0] = ""; buf[1] = "";
    for (i = 2; i < s; i++) {
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
    bool wait; //wait for ack
    struct sockaddr_in addr_con;
    int addrlen = sizeof(addr_con);
    addr_con.sin_family = AF_INET;
    addr_con.sin_port = htons(PORT);
    addr_con.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    char net_buf[SIZE]; char leg_buf[1]; char ack_buf[1];
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
	clearBuf(net_buf);
	seq = 0; //init seq
        printf("\nPlease enter file name to receive:\n");
        scanf("%s", net_buf);
	check_buf(net_buf);
	add_header(net_buf);
	check_buf(net_buf);

        sendto(sockfd, net_buf, SIZE,
               sendrecvflag, (struct sockaddr*)&addr_con,
               addrlen);
	//update legacy buffer
	wait = 1;
	while(wait){
	    //wait for ack
	    //run timeout timer
	   if(recvfrom(sockfd, ack_buf, 2, sendrecvflag, (struct sockaddr*)&addr_con, &addrlen) > 0){
		wait = 0;
		}
	}
        printf("\n---------Data Received---------\n");

        while (1) {
            // receive
            bzero(net_buf, SIZE);
            nBytes = recvfrom(sockfd, net_buf, SIZE,
                              sendrecvflag, (struct sockaddr*)&addr_con,
                              &addrlen);
            // process
            if (recvFile(net_buf, SIZE)) {
		//net_buf = strip_header(net_buf);
		fprintf(fp, strip_header(net_buf));
		fclose(fp);
                break;
            } else {
		//net_buf = strip_header(net_buf);
	        fprintf(fp, strip_header(net_buf));
	    }
        }
        printf("\n-------------------------------\n");
//    }
    return 0;
}
