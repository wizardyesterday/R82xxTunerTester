#include <stdio.h>
#include <string.h>

#include "IpcMessageQueue.h"

int main(int argc,char **argv)
{
  bool success;
  size_t bufferLength;
  size_t sendLength;
  char buffer[1024];
  char *messagePtr = "Client request\n";
  IpcMessageQueue *queuePtr;

  // Instantiate an IPC message queue object.
  queuePtr = new IpcMessageQueue("/home/chris/R82xxKeyFile",1,true,&success);

  if (!success)
  {
    // Release resources.
    delete queuePtr;

    // Exit program.
    return (-1);
  } // if

  // Account for the string terminator.
  sendLength = strlen(messagePtr) + 1; 

  // Send the message.
  success = queuePtr->sendData(1,messagePtr,sendLength);

  if (!success)
  {
    // Release resources.
    delete queuePtr;

    // Exit program.
    return (-2);
  } // if

  // Wait for response.
  success = queuePtr->receiveData(2,buffer,&bufferLength);

  if (success)
  {
  printf("bufferLength: %d\n",bufferLength);
  printf("message text: %s",buffer);
  } // if

  // Release resources.
  delete queuePtr;

  return (0);

} // main
