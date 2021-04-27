#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define SIZE 80
#define PORT 8080
#define SA struct sockaddr

//new variables for keeping track of everything
int total_packets_rec = 0;
int total_bytes_rec = 0;
int total_dup_packs = 0;
int total_dup_acks = 0;
int packets_lost = 0;
int rec_acks = 0;
int lost_acks = 0;
char filename[SIZE];


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

// Function designed for chat between client and server.
void chatfunc(int sockfd)
{
    char buff[SIZE];
    int n, control;
    control = 1;
    while(control != 0) {
        bzero(buff, SIZE);

        // read the message from client and copy it in buffer
        read(sockfd, buff, sizeof(buff));
	strcpy(filename, buff);

	//copy buffer contents then clear buffer
        bzero(buff, SIZE);
	if(filename != NULL){
	  control = 0;
	}
    }
}


void send_file(FILE *fp, int sockfd){
  int n, c, bytetotal, check = 0;
  char data[SIZE] = {0};
  n = 0; bytetotal = 0;


  //Okay so here is what needs to happen. We have to rewrite this while loop
  //at start we use recvfrom (https://linux.die.net/man/2/recvfrom)
  //in order to get the incoming packet from client
  //then we need to run the simulate packet loss function to see if we lose that packet
  //then we check the sequence number to verify packet is in order or if we got a dup
  //if we got a dup, then we go back to the top of the loop
  //else we send the client that single line
  // we currently still need to build a way to send acks (1 for good, 0 for bad)
  //seq number can just flip back and forth from 0 and 1.


  while((fgets(data, SIZE, fp)) != NULL) {
    check = NULL; //reset chkval
    check = write(sockfd, data, SIZE, 0);
    if(check > 0){
      n++; //only iterate n when packets are sent
    } else if (check == 0){
      printf("eot packet detected\n");
    }
     c = sizeof(data) / sizeof(data[0]);
     printf("Packet %d transmitted with %d data bytes\n", n, c);
     bytetotal += c;
     }
     printf("End of Transmission Packet with sequence number %d transmitted with %d data bytes\n", ++n, 0);
     printf("Number of data bytes transmitted: %d\n", bytetotal);
     bzero(data, SIZE);
}


int main(int argc, char *argv[]) //arg1 timeout value, arg2 packet loss rate, arg3 ack loss rate
{
	srand(time(0));
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    FILE *fp;

    //get rates from the args.
    double packet_loss_rate = atof(argv[1]);
    double ack_loss_rate = atof(argv[2]);

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("[!]Socket creation failed\n");
        exit(0);
    }
    else
        printf("[+]Socket successfully created\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("[+]Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("[!]Listen failed...\n");
        exit(0);
    }
    else
        printf("[+]Server listening...\n");
    len = sizeof(cli);

    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("[!]Server acccept failed\n");
        exit(0);
    }
    else
        printf("[+]Client has connected\n");

    // Chat with client to get filename
    chatfunc(connfd);
    printf("File '%s' requested by client.\n", filename);

    fp = fopen(filename, "r");//parse file
    if(fp != NULL){ printf("Responding...\n"); } else { printf("Could not find %s", filename); }//give feedback
    send_file(fp, connfd);//send file
    printf("[+]File sent.\n");

    // After chatting close the socket
    printf("Closing connection.\n");
    close(sockfd);
}
