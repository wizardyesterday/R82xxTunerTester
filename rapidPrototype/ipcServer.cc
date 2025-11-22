#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "IpcMessageQueue.h"

int main(int argc,char **argv)
{
  bool success;
  size_t bufferLength;
  size_t sendLength;
  char buffer[1024];
  char *messagePtr = "Server response\n";
  IpcMessageQueue *queuePtr;

  // Instantiate an IPC message queue object.
  queuePtr = new IpcMessageQueue("/home/chris/R82xxKeyFile",1,false,&success);

  if (!success)
  {
    // Release resources.
    delete queuePtr;

    // Exit program.
    return (-1);
  } // if

  // Wait for request message.
  success = queuePtr->receiveData(1,buffer,&bufferLength);

  if (success)
  {
    printf("bufferLength: %d\n",bufferLength);
    printf("message text: %s",buffer);
    // Account for the string terminator.
    sendLength = strlen(messagePtr) + 1; 

    // Send the message.
    success = queuePtr->sendData(2,messagePtr,sendLength);
  } // if

  // Release resources.
  delete queuePtr;

  return (0);

} // main
