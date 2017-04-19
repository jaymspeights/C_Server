/* A simple echo server in the internet domain using TCP and UDP
   The port number is passed as an argument
   Logs the message to a log server */
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>



//hard-coded port for server -> log_server communication
int log_portno = 9999;

void error(const char *msg) {
     perror(msg);
     exit(1);
}

void log(char msg[], int len) {
  //send message to log server
}

void doStuffTCP(int fd, char buff[], int buff_len) {
  int n;
  bzero(buff,buff_len);
  if ((n = read(fd,buff,buff_len-1)) < 0)
       error("ERROR reading from socket");
  //echo msg
  write(fd, buff, n);
  //log message

  close(fd);
}

void doStuffUDP(int fd, char buff[], int buff_len) {
  struct sockaddr_in remote;
  int len;

  bzero(buff,buff_len);
  len = recvfrom(fd, buff, buff_len - 1, 0, (struct sockaddr *)&remote, &len);
  buff[len] = '\0';

  //echo msg
  sendto(fd,buff,buff_len,0,(struct sockaddr *)&remote,len);

  //log msg
  log(buff, buff_len);
}

//waits with no hang to prevent zombie processes
void SigCatcher(int n) {
     wait3(NULL,WNOHANG,NULL);
}

int main(int argc, char *argv[]) {
     //buffer for recieved messages
     int buffer_length = 256;
     char buffer[buffer_length];

     int tcp_fd, udp_fd, newsockfd, portno;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;
     int n, pid;
     fd_set fset;

     //check for command line arguments
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     //get port from command line arugments
     portno = atoi(argv[1]);

     //init udp socket
     udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
     if (udp_fd < 0)
        error("ERROR opening socket");

     //init tcp socket
     tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
     if (tcp_fd < 0)
        error("ERROR opening socket");

     //init server address
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);

     //bind sockets
     if (bind(udp_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
              error("ERROR on binding");

     if (bind(tcp_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
              error("ERROR on binding");

     listen(tcp_fd,5);

     //catches SIGCHLD signal (child dies) and calls SigCatcher
     signal(SIGCHLD,SigCatcher);

     //reset the fd_set
     FD_ZERO(&fset);
     int maxfd = tcp_fd > udp_fd ? tcp_fd+1 : udp_fd+1;
     while(1) {
          //add the two sockets to the fd_set
          FD_SET(tcp_fd, &fset);
          FD_SET(udp_fd, &fset);
          //if no sockets are ready and
          if ((select(maxfd, &fset, NULL, NULL, NULL)) < 0) {
               //if interrupt flag is set, try again
               if (errno == EINTR)
                    continue;
               else
                    error("ERROR on select");
          }
          //if tcp is set
          if (FD_ISSET(tcp_fd, &fset)) {
              clilen = sizeof(cli_addr);
              newsockfd = accept(tcp_fd,(struct sockaddr *) &cli_addr,&clilen);
              if (newsockfd < 0)
                   error("ERROR on accept");
              if ((pid = fork()) == 0) //error on fork
              {
                   doStuffTCP(newsockfd, buffer, buffer_length);
                   exit(0);
              }
              close(newsockfd);

          }
          //if udp is set
          if (FD_ISSET(udp_fd, &fset)) {
              if ((pid = fork()) == 0)
              {
                   doStuffUDP(udp_fd, buffer, buffer_length);
                   exit(0);
              }
        }
     }
     close(tcp_fd);
     close(udp_fd);
     return 0;
}
