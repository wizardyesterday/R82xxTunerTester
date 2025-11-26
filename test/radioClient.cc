#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "R82xxTunerTester.h"
#include "IpcMessageQueue.h"

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// Local defines.
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// This structure is used to consolidate user parameters.
struct MyParameters
{
  int *startingIfGainPtr;
  int *endingIfGainPtr;
};

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// Global variables.
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// Static functions.
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
static bool keyPressed(void);

static bool sendIfGainCommand(int serverType,int ifGain,
  IpcMessageQueue *queuePtr);

static bool sendTerminateCommand(int serverType,IpcMessageQueue *queuePtr);
static bool waitForServerAck(int serverType,IpcMessageQueue *queuePtr);

/**************************************************************************

  Name: keyPressed

  Purpose: The purpose of this function is to determine of any key
  was pressed on the keyboard.

  Calling Sequence: status = keyPressed()
 
  Inputs:

    None.

  Outputs:

    status - An indicator of whether or not a key was pressed. A
    value of true indicates that a key was pressed, and a value of
    false indicates that a key was not pressed.

**************************************************************************/
static bool keyPressed(void)
{
  bool status;
  int stdinFileDescriptor;
  struct timeval timeout;
  fd_set readFds;

  // Default to no key pressed.
  status = false; 

  // Retrieve the file descriptor fore stdin file stream.
  stdinFileDescriptor = fileno(stdin);

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // This block of code is necessary when working with TCP.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Clear before use.
  FD_ZERO(&readFds);

  FD_SET(stdinFileDescriptor,&readFds);

  // set up timeout value to 200 milliseconds.
  timeout.tv_sec = 0;
  timeout.tv_usec = 200000;

  // wait and be nice to the system
  select(stdinFileDescriptor+1,&readFds,0,0,&timeout);
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  if (FD_ISSET(stdinFileDescriptor,&readFds))
  {
    // Indicate that a key was pressed.
    status = true;
  } // if

  return (status);

} // keyPressed

/**************************************************************************

  Name: sendIfGainCommand

  Purpose: The purpose of this function is to send a radio IF gain
  setting command to the radio server so that it  should marshall
  the information into a message to send to the radio diags app.

  Calling Sequence: success = sendIfGainCommand(serverType,
                                                ifGain, 
                                                queuePtr)
 
  Inputs:

    serverType - The type of server for which to wait on. 

    ifGain - The IF gain for which to set the radio to.

    queuePtr - A pointer to the messag queue.

  Outputs:

    success - A flag hat indicates whether or not the operation succeded
    or failed. A value of true indicates success, and a value of false
    indicates failure.

**************************************************************************/
static bool sendIfGainCommand(int serverType,
  int ifGain,
  IpcMessageQueue *queuePtr)
{
  bool success;
  int command;
  size_t bufferLength;
  char buffer[256];

  // Use the enumeration.
  command = RadioServerCmdSetIfGain;

  // Set up server command.
  snprintf(buffer,sizeof(buffer),"%d %d\n",command,ifGain);

  // Compute number of bytes to send.
  bufferLength = strlen(buffer) + 1;

  // Send commmand.
  success = queuePtr->sendData(serverType,buffer,bufferLength);

  return (success);

} // sendIfGainCommand

/**************************************************************************

  Name: sendTerminateCommand

  Purpose: The purpose of this function is to send a terminate command
  to the server so that it will exit.
  the information into a message to send to the radio diags app.

  fGainCalling Sequence: succes = sendTerminateCommand(serverType,
                                                       queuePtr)
 
  Inputs:

    serverType - The type of server for which to wait on. 

    queuePtr - A pointer to the messag queue.

  Outputs:

    success - A flag hat indicates whether or not the operation succeded
    or failed. A value of true indicates success, and a value of false
    indicates failure.

**************************************************************************/
static bool sendTerminateCommand(int serverType,IpcMessageQueue *queuePtr)
{
  bool success;
  int command;
  size_t bufferLength;
  char buffer[256];

  // Use the enumeration.
  command = CmdTerminate;

  // Set up server command.
  snprintf(buffer,sizeof(buffer),"%d\n",command);

  // Compute number of bytes to send.
  bufferLength = strlen(buffer) + 1;

  // Send commmand.
  success = queuePtr->sendData(serverType,buffer,bufferLength);

  return (success);

} // sendTerminateCommand

/**************************************************************************

  Name: waitForServerAck

  Purpose: The purpose of this function is to wait for the server
  to send an ack message.

  Calling Sequence: = success = waitForServerAck(serverType,queuePtr)
 
  Inputs:

    serverType - The type of server for which to wait on. 

    queuePtr - A pointer to the messag queue.

  Outputs:

    success - A flag hat indicates whether or not the operation succeded
    or failed. A value of true indicates success, and a value of false
    indicates failure.

**************************************************************************/
static bool waitForServerAck(int serverType,IpcMessageQueue *queuePtr)
{
  bool success;
  size_t bufferLength;
  char buffer[80];

  // Wait for ack with no payload.
  success = queuePtr->receiveData(RadioServerTypeAck,
                                  buffer,
                                  &bufferLength);

  return (success);

} // waitForServerAck

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// End of static functions.
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

/*****************************************************************************

  Name: getUserArguments

  Purpose: The purpose of this function is to retrieve the user arguments
  that were passed to the program.  Any arguments that are specified are
  set to reasonable default values.

  Calling Sequence: exitProgram = getUserArguments(parameters)

  Inputs:

    parameters - A structure that contains pointers to the user parameters.

  Outputs:

    exitProgram - A flag that indicates whether or not the program should
    be exited.  A value of true indicates to exit the program, and a value
    of false indicates that the program should not be exited..

*****************************************************************************/
bool getUserArguments(int argc,char **argv,struct MyParameters parameters)
{
  bool exitProgram;
  bool done;
  int i;
  int opt;

  // Default not to exit program.
  exitProgram = false;

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Default parameters.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // The radio starting and ending IF gains to cycle through.
  *parameters.startingIfGainPtr = 0;
  *parameters.endingIfGainPtr = 4;
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  // Set up for loop entry.
  done = false;

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Retrieve the command line arguments.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  while (!done)
  {
    // Retrieve the next option.
    opt = getopt(argc,argv,"S:E:h");

    switch (opt)
    {
      case 'S':
      {
         // Retrieve the starting IF gain.
        *parameters.startingIfGainPtr = atoi(optarg);
        break;
      } // case

      case 'E':
      {
         // Retrieve the ending IF gain.
        *parameters.endingIfGainPtr = atoi(optarg);
        break;
      } // case

      case 'h':
      {
        // Display usage.
        fprintf(stderr,"./tcpClient -S <startingIfGain -E <endingifGain\n");

        // Indicate that program must be exited.
        exitProgram = true;
        break;
      } // case

      case -1:
      {
        // All options consumed, so bail out.
        done = true;
      } // case

    } // switch

  } // while
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  return (exitProgram);

} // getUserArguments

//***********************************************************************
// Mainline code.
//***********************************************************************
int main(int argc,char **argv)
{
  // Miscellaneous support.
  bool success;
  bool done;
  bool exitProgram;
  int ifGain;
  int startingIfGain;
  int endingIfGain;
  struct MyParameters parameters;
  char inputBuffer[256];

  // Message queue support.
  IpcMessageQueue *queuePtr;
  char *chPtr;
  
  // Set up for parameter transmission.
  parameters.startingIfGainPtr = &startingIfGain;
  parameters.endingIfGainPtr = &endingIfGain;

  // Retrieve the system parameters.
  exitProgram = getUserArguments(argc,argv,parameters);

  if (exitProgram)
  {
    // Bail out.
    return (0);
  } // if

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

  // Initialize.
  ifGain = startingIfGain;

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
          success = sendIfGainCommand(RadioServerTypeCmd,ifGain,queuePtr);

          if (success)
          {
            // Qait for the server ack.
            success = waitForServerAck(RadioServerTypeAck,queuePtr);
          } //  if

          // Increment the IF gain.
          ifGain++;

          if (ifGain > endingIfGain)
          {
            // Wrap it.
            ifGain = startingIfGain;
          } // if

          break;
        } // case

        case 'e':
        {
          // Notify the server to exit.
          success = sendTerminateCommand(RadioServerTypeCmd,queuePtr);

          // Acks are not sent when a server is told to terminate..

          // We're done, so let's bail out.
          done = true;

          break;
        } // case

       default:
        {
          break;
        } // case

      } // switch

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

