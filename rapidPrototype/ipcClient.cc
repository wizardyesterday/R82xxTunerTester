#include <stdio.h>
#include <string.h>

#include "IpcMessageQueue.h"

//************************************************************
// This program implements a client that illustrates
// the use of System V message queues. It implements a
// stop and wait protocol so that the server program
// will block on its message queue waiting for a request
// message to arrive. After this client sends a message, it
// will block on the message queue waiting for a server
// response message.  The result is that all transactions
//  will be synchronized.
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

  // Instantiate a message queue.
  queuePtr = new IpcMessageQueue("/home/chris/R82xxKeyFile",1,true,&success);

  if (!success)
  {
    // Release resources.
    delete queuePtr;

    // Exit program.
    return (-1);
  } // if

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // This block of code sends a request message to
  // the server.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
   // Set the message.
   messagePtr = "Client request\n";

  // Account for the string terminator.
  sendLength = strlen(messagePtr) + 1;

  // Send the message.
  success = queuePtr->sendData(1,messagePtr,sendLength);
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  if (!success)
  {
    // Release resources.
    delete queuePtr;

    // Exit program.
    return (-2);
  } // if
 
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // This block of code blocks on the message queue
  // until a server response arrives.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Wait for response.
  success = queuePtr->receiveData(2,buffer,&bufferLength);

  if (success)
  {
  printf("bufferLength: %d\n",bufferLength);
  printf("message text: %s",buffer);
  } // if
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  // Release resources.
  delete queuePtr;

  return (0);

} // main
