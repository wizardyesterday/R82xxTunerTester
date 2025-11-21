#include <stdio.h>
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

  // Generate unique key;
  key = ftok("keyFile",1); 

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

  // Seet up a message.
  sendBuffer.mtype = 1;
  sendBuffer.mtext[0] = 'S';  
  sendBuffer.mtext[1] = 0;

  // Send the message.
  msgsnd(mqid,&sendBuffer,2,0);

  // Dequeue the message.
  bufferLength = msgrcv(mqid,&receiveBuffer,20,1,0);

  printf("bufferLength: %d\n",bufferLength);
  printf("message type: %d\n",receiveBuffer.mtype);
  printf("message text: %s\n",receiveBuffer.mtext);

  // Destroy the queue since we don't want it lingering.
  msgctl(mqid,IPC_RMID,NULL);

  return (0);

} // main
