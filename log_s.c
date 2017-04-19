/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

void error(const char *msg)
{
     perror(msg);
     exit(1);
}

//waits with no hang to prevent zombie proccesses
void SigCatcher(int n)
{
     wait3(NULL,WNOHANG,NULL);
}

int main(int argc, char *argv[])
{
     int sockfd, portno;
     int buffer_length = 256;
     char buffer[buffer_length];
     struct sockaddr_in serv_addr;
     int n, pid;

     //catches SIGCHLD signal (child dies) and calls SigCatcher
     signal(SIGCHLD,SigCatcher);

     //init socket
     sockfd = socket(AF_INET, SOCK_DGRAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");

     //hard-coded port for server -> log_server communication
     portno = 9999;

     //init server details
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);

     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");

     //changes stdout to output to this file
     int log_fd = open("echo.log", O_W | O_CREAT, S_IRUSR)
     dup2(log_fd, 1);

     int length;

     //loop to accept clients and fork
     while(1)
     {
          bzero(buffer,buffer_length);
          length = recvfrom(sockfd, buffer,
                      sizeof(buffer) - 1,
                      0, NULL, 0;
          if (length < 0)
               error("ERROR on recv");
          pid = fork();

          if (pid < 0) //error on fork
               error("ERROR on fork");

          else if (pid == 0) //child proccess
          {
               close(sockfd);
               time_t t = time(NULL);
               struct tm tm = *localtime(&t);
               buffer[length] = "\0";
               printf("%d-%d-%d %d:%d:%d\t%s\n",
                         tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                         tm.tm_hour, tm.tm_min, tm.tm_sec, buffer);
               exit(0);
          }

          else //parent proccess
          {
               length = -1;

          }
     }
     close(sockfd);
     return 0;
}