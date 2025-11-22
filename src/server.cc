#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct messageBuffer
{
  long mtype;
  char mtext[128];
};

int main(int argc,char **argv)
{
  key_t key;
  int mqid;
  messageBuffer sendBuffer;
  messageBuffer receiveBuffer;
  size_t bufferLength;
  int sendLength;
  char *messagePtr = "Server response\n";

  // Generate unique key;
  key = ftok("/home/chris/R82xxKeyFile",1); 

  if (key == -1)
  {
    fprintf(stderr,"ERROR: ftok()\n");
    return (-1);
  } // if

  // Create a message queue or use one if it already exists.
  mqid = msgget(key,IPC_CREAT | 0666);

  if (mqid == -1)
  {
    fprintf(stderr,"ERROR: msgget()\n");
    return (-2);
  } // if

  // Wait for request message.
  bufferLength = msgrcv(mqid,&receiveBuffer,100,1,0);

  printf("bufferLength: %d\n",bufferLength);
  printf("message type: %d\n",receiveBuffer.mtype);
  printf("message text: %s",receiveBuffer.mtext);

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // This block of code sets up and sends a response.
  // It can be  used to throttle the client requests.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Set up the response
  sendBuffer.mtype = 2;
  strcpy(sendBuffer.mtext,messagePtr);

  // Account for the string terminator.
  sendLength = strlen(sendBuffer.mtext) + 1;

  // Send the response.
  msgsnd(mqid,&sendBuffer,strlen(messagePtr),0);
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  return (0);

} // main
