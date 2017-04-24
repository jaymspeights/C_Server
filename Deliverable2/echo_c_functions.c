//Supplementary functions for client.c
//Written by James Speights

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void error(char *msg){
    perror(msg);
    exit(0);
}

void dostuffTCP(int fd, char *buffer[]){
  int n = write(fd,buffer,strlen(buffer));
  if (n < 0)
       error("ERROR writing to socket");
  bzero(buffer,256);
  n = read(fd,buffer,255);
  if (n < 0)
       error("ERROR reading from socket");
  printf("%s\n",buffer);
  close(fd);
}

void dostuffUDP(int fd, char *buffer){
  send(fd, buffer, strlen(buffer), 0);
  int length;
  length = recv(fd, buffer, strlen(buffer), 0);
  buffer[length] = '\0';
  printf("Server response: %s\n", buffer);
  close(fd);
}
