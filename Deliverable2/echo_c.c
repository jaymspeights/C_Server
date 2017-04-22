//A simple client that can comminucate through TCP or UDP
//Run the executable with arguments [hostname] [port]

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "client_functions.c"

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);

    printf("TCP or UDP?: (t : u)\n");
    char choice;
    choice = fgetc(stdin);

    portno = atoi(argv[2]);

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    while (1) {
      //tcp connection
      if (choice == 't') {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
            error("ERROR opening socket");

        if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
            error("ERROR connecting");

        dostuffTCP(sockfd, buffer);
        exit(0);
      }
      //udp connection
      if (choice == 'u') {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0)
            error("ERROR opening socket");

        if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
            error("ERROR connecting");

        dostuffUDP(sockfd, buffer);
        exit(0);
      }
      printf("Please enter t for TCP or u for UDP:\n");
      char choice;
      choice = fgetc(stdin);
    }

    return 0;
}
