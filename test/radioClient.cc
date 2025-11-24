#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "R82xxTunerTester.h"
#include "TcpClient.h"
#include "IpcMessageQueue.h"


//***********************************************************************
// Mainline code.
//***********************************************************************
int main(int argc,char **argv)
{
  // Miscellaneous support.
  bool success;
  bool done;
  char inputBuffer[256];

  // Message queue support.
  int ifGain;
  IpcMessageQueue *queuePtr;
  char *chPtr;
  char queueBuffer[16384];
  size_t queueBufferLength;
  int sendLength;
  
  // Set to something valid.
  ifGain = 20;

  // Instantiate an IPC message queue object.
  queuePtr = new IpcMessageQueue(KEYFILE,
                                  PROJECT_ID,
                                  true,
                                  &success);
  if (!success)
  {
    return (-1);
  } // if
  // Set up for loop entry.
  done = false;

  while (!done)
  {
    // Get user command)
    chPtr = fgets(inputBuffer,80,stdin);

    if (chPtr != NULL) 
    {
      switch(inputBuffer[0])
      {
        case 'r':
        {
          // Set up radio server command payload.
          sprintf(queueBuffer,"1 %d\n",ifGain);
          queueBufferLength = strlen(queueBuffer) + 1;

          break;
        } // case

        case 'e':
        {

          // Set up radio server command payload.
          sprintf(queueBuffer,"2");
          queueBufferLength = strlen(queueBuffer) + 1;

          // Bail out of loop.
          done = true;
          break;
        } // case

      } // switch

      // Send commmand.
      success = queuePtr->sendData(RadioServerTypeCommand,
                                   queueBuffer,
                                   queueBufferLength);

      if (success)
      {
        // Wait for ack with no payload.
        success = queuePtr->receiveData(RadioServerTypeAck,
                                     queueBuffer,
                                     &queueBufferLength);
      } // if

      if (!success)
      {
        // Bail out of loop.
        done = true;
      } // if
  
    } // if
    else
    {
      // Bail out of loop.
      done = true;
    } // else

  } // while
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  // Release resources.
  delete queuePtr;

  return (0);

} // main

