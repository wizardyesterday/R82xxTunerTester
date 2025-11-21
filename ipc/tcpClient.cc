#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc,char **argv)
{
  int result;
  int sock;
  struct sockaddr_in server_addr;
  char buffer[16384];

  // 1. Create socket.
  sock = socket(AF_INET,SOCK_STREAM,0);

  if (sock  < 0)
  {
    fprintf(stderr,"ERROR, socket()\n");
    return (-1);
  } // if

  // Configure server address.
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(20280);
  inet_pton(AF_INET,"192.93.16.74",&server_addr.sin_addr);

  // 2. Connect to server.
  result = connect(sock,(struct sockaddr *)&server_addr,sizeof(server_addr));

  if (result < 0)
  {
    fprintf(stderr,"ERROR: connect()\n");
    return (-2);
  } // if

  printf("Connected to server\n");

  // Receive diags welcome message.
  memset(buffer,0,sizeof(buffer));
  recv(sock,buffer,sizeof(buffer)-1,0);

  printf("%s\n",buffer);

  // Send a diags command.
  strcpy(buffer,"get agcinfo\n");
  send(sock,buffer,strlen(buffer),0);

  // Read the response.
  recv(sock,buffer,sizeof(buffer)-1,0);

  printf("%s\n",buffer);
  // Close the connection.
  close(sock);

  printf("Connection closed\n");  

  return (0);

} // main
