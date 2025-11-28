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
  int *startingTagPtr;
  int *endingTagPtr;
  bool *automaticModePtr;
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

static bool sendPowerCommand(int serverType,int tag,
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

  Name: sendPowerCommand

  Purpose: The purpose of this function is to send a power measurement
  command to the spectrum server so.

  Calling Sequence: success = sendPowerCommand(serverType,
                                                tag,
                                                queuePtr)

  Inputs:

    serverType - The type of server for which to send data.

    tag - The tag field for which to write to an output file record.

    queuePtr - A pointer to the messag queue.

  Outputs:

    success - A flag hat indicates whether or not the operation succeded
    or failed. A value of true indicates success, and a value of false
    indicates failure. 

**************************************************************************/
static bool sendPowerCommand(int serverType,
  int tag,
  IpcMessageQueue *queuePtr)
{
  bool success;
  int command;
  size_t bufferLength;
  char buffer[256];

  // Use the enumeration.
  command = SpectrumServerCmdComputePower;

  // Set up server command.
  snprintf(buffer,sizeof(buffer),"%d %d\n",command,tag);

  // Compute number of bytes to send.
  bufferLength = strlen(buffer) + 1;

  // Send commmand.
  success = queuePtr->sendData(serverType,buffer,bufferLength);

  return (success);

} // sendPowerCommand

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
  success = queuePtr->receiveData(serverType,
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
  // The radio starting and ending IF gains to span.
  *parameters.startingIfGainPtr = 0;
  *parameters.endingIfGainPtr = 4;

  // The starting and ending tags to sspan.
  *parameters.startingTagPtr = 0;
  *parameters.endingTagPtr = 4;

  // Default to manual mode.
  parameters.automaticModePtr = false;
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  // Set up for loop entry.
  done = false;

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Retrieve the command line arguments.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  while (!done)
  {
    // Retrieve the next option.
    opt = getopt(argc,argv,"L:U:S:E:h");

    switch (opt)
    {

      case 'L':
      {
         // Retrieve the starting IF gain.
        *parameters.startingIfGainPtr = atoi(optarg);
        break;
      } // case

      case 'U':
      {
         // Retrieve the ending IF gain.
        *parameters.endingIfGainPtr = atoi(optarg);
        break;
      } // case

      case 'S':
      {
         // Retrieve the starting tag.
        *parameters.startingTagPtr = atoi(optarg);
        break;
      } // case

      case 'E':
      {
         // Retrieve the ending tag.
        *parameters.endingTagPtr = atoi(optarg);
        break;
      } // case

      case 'A':
      {
         // Retrieve the automatic mode flag.
        *parameters.automaticModePtr = atoi(optarg);
        break;
      } // case

      case 'h':
      {
        // Display usage.
        fprintf(stderr,"./systemCoordinator -L <startingIfGain "
                       "-U <endingifGain\n"
                       "                    -S <startingTag> -E <endingTag>\n"
                       "                    -A automaticMode\n");

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
  int tag;
  int startingTag;
  int endingTag;
  bool automaticMode;

  struct MyParameters parameters;
  char inputBuffer[256];

  // Message queue support.
  IpcMessageQueue *queuePtr;
  char *chPtr;
  
  // Set up for parameter transmission.
  parameters.startingIfGainPtr = &startingIfGain;
  parameters.endingIfGainPtr = &endingIfGain;
  parameters.startingTagPtr = &startingTag;
  parameters.endingTagPtr = &endingTag;
  parameters.automaticModePtr = &automaticMode;

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
  tag = startingTag;

  printf("Coordinator up and running\n");
  printf("Press <m> to measure, <e> to exit\n");

  while (!done)
  {
    printf("tag: %d    ifGain: %d\n",tag,ifGain);

    // Get user command)
    chPtr = fgets(inputBuffer,80,stdin);

    if (chPtr != NULL) 
    {
      switch(inputBuffer[0])
      {
        case 'm':
        {
          success = sendIfGainCommand(RadioServerTypeCmd,ifGain,queuePtr);

          if (success)
          {
            // Wait for the server ack.
            success = waitForServerAck(RadioServerTypeAck,queuePtr);
          } //  if

          // Increment the IF gain.
          ifGain++;

          if (ifGain > endingIfGain)
          {
            // Wrap it.
            ifGain = startingIfGain;
          } // if

          success = sendPowerCommand(SpectrumServerTypeCmd,tag,queuePtr);

          if (success)
          {
            // Wait for the server ack.
            success = waitForServerAck(SpectrumServerTypeAck,queuePtr);
          } //  if

          // Increment the tag;
          tag++;

          if (tag > endingTag)
          {
            // Wrap it.
            tag = startingTag;
          } // if

          break;
        } // case

        case 'e':
        {
          // Notify the servers to exit.
          success = sendTerminateCommand(RadioServerTypeCmd,queuePtr);
          success = sendTerminateCommand(SpectrumServerTypeCmd,queuePtr);

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

