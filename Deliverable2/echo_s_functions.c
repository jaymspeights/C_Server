//Supplementary functions for echo_s_functions.c

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

void doStuffTCP(int fd, char buff[], int buff_len) {
  int n;
  bzero(buff,buff_len);
  if ((n = read(fd,buff,buff_len-1)) < 0)
       error("ERROR reading from socket");

  printf("Got message: %s", buff);
  //echo msg
  write(fd, buff, n);
  close(fd);

  //log
  log(buff);
}

void doStuffUDP(int fd, char buff[], int buff_len) {
  struct sockaddr_in remote;
  int len, length;
  len = sizeof(remote);
  bzero(buff,buff_len);
  length = recvfrom(fd, buff, buff_len - 1, 0, (struct sockaddr *)&remote, &len);
  buff[length] = '\0';
  printf("Got message: %s", buff);

  //echo msg
  sendto(fd,buff,length,0,(struct sockaddr *)&remote,len);

  //log
  log(buff);
}

//logs the data to the log server
void log(char buff[]){
     struct sockaddr_in remote;
     sendto(gethostbyname("localhost"), buff, buff.length-1, (struct sockaddr *)&remote, sizeof(remote));
}

//waits with no hang to prevent zombie processes
void SigCatcher(int n) {
     wait3(NULL,WNOHANG,NULL);
}
