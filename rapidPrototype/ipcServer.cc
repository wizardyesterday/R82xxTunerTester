#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "IpcMessageQueue.h"

//************************************************************
// This program implements a server that illustrates
// the use of System V message queues. It implements a
// stop and wait protocol so that the client program
// will block on its message queue after it sends a
// request to this server. The result is that all
// transactions will be synchronized.
//************************************************************

//************************************************************
// Mainline code.
//************************************************************
int main(int argc,char **argv)
{
  bool success;
  size_t bufferLength;
  size_t sendLength;
  char buffer[1024];
  char *messagePtr;
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

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // This block of code blocks on the message queue
  // until a client request arrives.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Wait for request message.
  success = queuePtr->receiveData(1,buffer,&bufferLength);

  if (success)
  {
    printf("bufferLength: %d\n",bufferLength);
    printf("message text: %s",buffer);
  } // if
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  if (success)
  {
    //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
    // This block of code sends a response message to
    // the client.
    //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
    // Set the message.
    messagePtr = "Server Response\n";

    // Account for the string terminator.
    sendLength = strlen(messagePtr) + 1; 

    // Send the message.
    success = queuePtr->sendData(2,messagePtr,sendLength);
    //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  } // if

  // Release resources.
  delete queuePtr;

  return (0);

} // main
