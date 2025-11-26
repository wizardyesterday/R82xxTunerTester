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
#define DEFAULT_FILENAME "mySpectrumFile.txt"

// This structure is used to consolidate user parameters.
struct MyParameters
{
  char *filenamePtr;
  float *bandwidthInHzPtr;
};

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// Global variables.
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
float bandwidthInHz;

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// Static functions.
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
static bool keyPressed(void);
static bool decodeMessageQueueCommand(char *bufferPtr,char *filenamePtr);

/**************************************************************************

  Name: keyPressed

  Purpose: The purpose of this function is to determine of any key
  was pressed on the keyboard.

  Calling Sequence: status = keyPressed();
 
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

  Name: decodeMessageQueueCommand

  Purpose: The purpose of this function is to decode and process a
  message that was removed from the message queue.

  Calling Sequence: = decodeMessageQueueCnate ommand(bufferPtr)
 
  Inputs:

    bufferPtr - A pointer to a buffer that references the command and
    its associated parameters.  

  Outputs:

    terminate - A flag, that if set to true, indicates that a terminate
    message arrived.

**************************************************************************/
static bool decodeMessageQueueCommand(char *bufferPtr,char *filenamePtr)
{
  bool terminate;
  int count;
  int opcode;
  int tag;
  char commandBuffer[4096];

  // Indicate that a terminate message was not received.
  terminate = false;

  count = sscanf(bufferPtr,"%d",&opcode);

  if (count > 0)
  {
    switch (opcode)
    {
      case SpectrumServerCmdComputePower:
      {
        count = sscanf(bufferPtr,"%d %d %f",&opcode,&tag,&bandwidthInHz);

        // All parameters have been sent.
        if (count == 2)
        {
          // Construct radio command string.
          snprintf(commandBuffer,sizeof(commandBuffer),
                   "netcat -l -u -p 8001 | ./spectrumProcessor -B %f "
                   "-t %d >> %s\n",bandwidthInHz,tag,filenamePtr);

          // Execute the command.
          printf(commandBuffer);
          system(commandBuffer);
        } // if

        break;
      } // case

      case CmdTerminate:
      {
        // Indicate that a terminate message was received.
        terminate = true;

        break;
      } // case

      default:
      {
        break;
      } // case

    } // switch

  } // if

  return (terminate);

} // decodeMessageQueueCommand

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
  // The output file name.
  strcpy(parameters.filenamePtr,DEFAULT_FILENAME);

  // Default bandwidth.
  *parameters.bandwidthInHzPtr = 500;
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  // Set up for loop entry.
  done = false;

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Retrieve the command line arguments.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  while (!done)
  {
    // Retrieve the next option.
    opt = getopt(argc,argv,"f:b:h");

    switch (opt)
    {
      case 'f':
      {
        // Retrieve the file name.
        strcpy(parameters.filenamePtr,optarg);
        break;
      } // case

      case 'b':
      {
        // Retrieve the bandwidth.
         *parameters.bandwidthInHzPtr = atof(optarg);
        break;
      } // case

      case 'h':
      {
        // Display usage.
        fprintf(stderr,"./spectrumServer -f <filename> -b <bandwidthInHz\n");

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
  char filename[1024];
  struct MyParameters parameters;
  char *chPtr;

  // Message queue support.
  IpcMessageQueue *queuePtr;
  char queueBuffer[16384];
  size_t queueBufferLength;
  int sendLength;

  // Set up for parameter transmission.
  parameters.filenamePtr = filename;
  parameters.bandwidthInHzPtr = &bandwidthInHz;

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
                                  false,
                                  &success);

  if (!success)
  {
    // Release resources.
    delete queuePtr;
    
    // Exit program.
    return (0);
  } // if

  // Set up for loop entry.
  done = false;

  while (!done)
  {
    // Wait for a commmand.
    success = queuePtr->receiveData(SpectrumServerTypeCmd,
                                   queueBuffer,
                                   &queueBufferLength);

    if (success)
    {
      // Decode the message from the spectrum client.
     done = decodeMessageQueueCommand(queueBuffer,filename);

      if (!done)
      {
        // Send an ack with no payload.
        success = queuePtr->sendData(SpectrumServerTypeAck,
                                     queueBuffer,
                                     0);
      } // if
    } // if

    if (!success)
    {
      // Bail out of loop.
      done = true;
    } // if
  
  } // while
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  // Release resources.
  delete queuePtr;

  return (0);

} // main

