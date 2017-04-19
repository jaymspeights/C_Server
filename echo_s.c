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

void error(const char *msg) {
     perror(msg);
     exit(1);
}

void doStuffTCP(char *buff[]) {
     //implement echo
     //log message
}

void doStuffUDP(char *buff[]) {
     //implement echo
     //log message
}

//waits with no hang to prevent zombie processes
void SigCatcher(int n) {
     wait3(NULL,WNOHANG,NULL);
}

int main(int argc, char *argv[]) {
     int tcp_fd, udp_fd, newsockfd, portno;
     socklen_t clilen;
     int buffer_length = 256;
     char buffer[buffer_length];
     struct sockaddr_in serv_addr_tcp, serv_addr_udp;
     int n, pid;
     fd_set fset;

     //hard-coded port for server -> log_server communication
     int log_portno = 9999;

     //catches SIGCHLD signal (child dies) and calls SigCatcher
     signal(SIGCHLD,SigCatcher);

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
     bzero((char *) &serv_addr_udp, sizeof(serv_addr_udp));
     serv_addr_udp.sin_family = AF_INET;
     serv_addr_udp.sin_addr.s_addr = INADDR_ANY;
     serv_addr_udp.sin_port = htons(portno);

     if (bind(udp_fd, (struct sockaddr *) &serv_addr_udp,
              sizeof(serv_addr_udp)) < 0)
              error("ERROR on binding");


     //init tcp socket
     tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
     if (tcp_fd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr_tcp, sizeof(serv_addr_tcp));
     serv_addr_tcp.sin_family = AF_INET;
     serv_addr_tcp.sin_addr.s_addr = INADDR_ANY;
     serv_addr_tcp.sin_port = htons(portno);

     if (bind(tcp_fd, (struct sockaddr *) &serv_addr_tcp,
              sizeof(serv_addr_tcp)) < 0)
              error("ERROR on binding");

     listen(tcp_fd,5);

     //reset the fd_set
     FD_ZERO(&fset);
     int maxfd = max(tcp_fd, udp_fd) + 1;
     while(1) {
          //add the two sockets to the fd_set
          FD_SET(tcp_fd, &fset);
          FD_SET(udp_fd, &fset);
          //if no sockets are ready and
          if ((num_ready = select(maxfd, &fset, NULL, NULL, NULL)) < 0) {
               //if interrupt flag is set, try again
               if (errno == EINTR)
                    continue;
               else
                    error("ERROR on select");
          }
          //if tcp is set
          if (FD_ISSET(tcp_fd, &fset)) {
              clilen = sizeof(cli_addr);
              newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
              if (newsockfd < 0)
                   error("ERROR on accept");
              if ((pid = fork()) == 0) //error on fork
              {
                   close(tcp_fd);
                   bzero(buffer,256);
                   n = read(newsockfd,buffer,255);
                   if (n < 0)
                        error("ERROR reading from socket");
                   doStuffTCP(buffer);
                   exit(0);
              }
              close(newsockfd);

          }
          //if udp is set
          if (FD_ISSET(udp_fd, &fset)) {
              bzero(buffer,buffer_length);
              length = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, NULL, 0);
              if (length < 0)
                   error("ERROR on recv");

              if ((pid = fork()) == 0)
              {
                   buffer[length] = "\0";
                   doStuffUDP(buffer);
              }
              length = -1;
        }
     }
     close(tcp_fd);
     close(udp_fd);
     return 0;
}
