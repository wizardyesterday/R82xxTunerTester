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

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// Local defines.
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#define DEFAULT_SERVER_IP_ADDRESS "192.93.16.87"
#define DEFAULT_SERVER_PORT (8001)
#define DEFAULT_FREQUENCY (57600000)

// This structure is used to consolidate user parameters.
struct MyParameters
{
  char *serverIpAddressPtr;
  int *serverPortPtr;
  float *frequencyInHzPtr;
};

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// Global variables.
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
TcpClient *networkInterfacePtr;

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// Static functions.
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
static bool keyPressed(void);
static void sendRadioCommand(char *commandPtr,uint32_t delayInMilliseconds);
static bool decodeMessageQueueCommand(char *bufferPtr);

/**************************************************************************

  Name: keyPressed

  Purpose: The purpose of this function is to determine of any key
  was pressed on the keyboard.

  Calling Sequence: status = status = keyPressed();
 
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

  Name: sendRadioCommand

  Purpose: The purpose of this function is to send to send a command
  to the radio diags application that communicates with an rtl-sdr
  radio.

  Calling Sequence: status = sendRadioCommand(commandPtr,
                                              delayInMilliseconds)
 
  Inputs:

    commandPtr - A pointer to a command string that is to be sent
    to the radio.

    delayInMilliseconds - The delay for which to wait before
    reading a response from the radio as a result of the command
    sent. A delay value of zero will result in no delay occurring.

  Outputs:

    None.

**************************************************************************/
static void sendRadioCommand(char *commandPtr,uint32_t delayInMilliseconds)
{
  uint32_t delayInMicroseconds;
  int octetCount;
  char *networkBufferPtr;
  char networkOutputBuffer[16384];

  // Echo the command to the console.
  printf("%s",commandPtr);

  // Convert delay from milliseconds to microseconds.
  delayInMicroseconds = delayInMilliseconds * 1000;

  // Compute number of octets to send.
  octetCount = strlen(commandPtr);

  // Send the command to the radio.
  octetCount = networkInterfacePtr->sendData(commandPtr,octetCount);

  if (delayInMicroseconds > 0)
  {
    // Wait until the delay time expires.
    usleep(delayInMicroseconds);
  } // if

  // Retrieve radio response.
  octetCount = networkInterfacePtr->receiveData(networkOutputBuffer,16000);

  // Terminate the string, and output  the data to the console.
  networkOutputBuffer[octetCount] = 0;
  printf("%s",networkOutputBuffer);

  return;

} // sendRadioCommand

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
static bool decodeMessageQueueCommand(char *bufferPtr)
{
  bool terminate;
  int count;
  int opcode;
  int ifGain;
  char commandBuffer[80];

  // Indicate that a terminate message was not received.
  terminate = false;

  count = sscanf(bufferPtr,"%d",&opcode);

  if (count > 0)
  {
    switch (opcode)
    {
      case RadioServerCmdSetIfGain:
      {
        count = sscanf(bufferPtr,"%d %d",&opcode,&ifGain);

        // All parameters have been sent.
        if (count == 2)
        {
          // Construct radio command string.
          snprintf(commandBuffer,sizeof(commandBuffer),
                   "set rxifgain %d\n",ifGain);

          // Send the command.
          sendRadioCommand(commandBuffer,0);
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
  // The server IP address.
  strcpy(parameters.serverIpAddressPtr,DEFAULT_SERVER_IP_ADDRESS);
 
  // The server listener port.
  *parameters.serverPortPtr = DEFAULT_SERVER_PORT;

  // The default radio frequency.
  *parameters.frequencyInHzPtr = DEFAULT_FREQUENCY; 
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  // Set up for loop entry.
  done = false;

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Retrieve the command line arguments.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  while (!done)
  {
    // Retrieve the next option.
    opt = getopt(argc,argv,"a:p:f:h");

    switch (opt)
    {
      case 'a':
      {
        // Retrieve the IP address.        
        strcpy(parameters.serverIpAddressPtr,optarg);
        break;
      } // case

      case 'p':
      {
        // Retrieve the host listener port.
        *parameters.serverPortPtr = atoi(optarg);
        break;
      } // case

      case 'f':
      {
        // Retrieve the frequency.
        *parameters.frequencyInHzPtr = atof(optarg);

        if (*parameters.frequencyInHzPtr <= 0)
        
          // Keep it sane.
          {
          *parameters.frequencyInHzPtr = DEFAULT_FREQUENCY; 
        } // if 
        break;
      } // case

      case 'h':
      {
        // Display usage.
        fprintf(stderr,"./radioserver -a <serverIpAddress: x.x.x.x> "
                "-p <serverPort> -f <frequencyInHz>\n");

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
  float frequencyInHz;
  struct MyParameters parameters;
  char *chPtr;
  char commandBuffer[100];

  // Message queue support.
  IpcMessageQueue *queuePtr;
  char queueBuffer[16384];
  size_t queueBufferLength;
  int sendLength;

  // Tcp client support.
  int serverPort;
  char serverIpAddress[32];
  int octetCount;
  char networkOutputBuffer[16384];
  
  // Set up for parameter transmission.
  parameters.serverPortPtr = &serverPort;
  parameters.serverIpAddressPtr = serverIpAddress;
  parameters.frequencyInHzPtr = &frequencyInHz;

  // Retrieve the system parameters.
  exitProgram = getUserArguments(argc,argv,parameters);

  if (exitProgram)
  {
    // Bail out.
    return (0);
  } // if

  // Create TCP client object.
  networkInterfacePtr = new TcpClient(serverIpAddress,
                                      serverPort,
                                      &success);

  if (!success)
  {
    fprintf(stderr,"Failed to establish link\n");
    return (-1);
  } // if

  // Display diags welcome messate.
  octetCount = networkInterfacePtr->receiveData(networkOutputBuffer,16000);

  // Terminate the returned string, and print it.
  networkOutputBuffer[octetCount] = 0;
  printf("%s",networkOutputBuffer);

  // Set up radio.
  sendRadioCommand("set demodmode 0\n",0);
  sendRadioCommand("enable iqdump\n",0);
  sendRadioCommand("set agclevel -8\n",0);
  sendRadioCommand("set rxifgain 0\n",0);
  sendRadioCommand("set rxgain 50\n",0);
  sendRadioCommand("start receiver\n",3000);
  sendRadioCommand("start ringoscillator 8 48 0\n",0);

  snprintf(commandBuffer,sizeof(commandBuffer),
           "set rxfrequency %.0f\n",frequencyInHz); 

  sendRadioCommand(commandBuffer,0);

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Radio is set up for testing, so now message queue sruff
  // is set up.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Instantiate an IPC message queue object.
  queuePtr = new IpcMessageQueue(KEYFILE,
                                  PROJECT_ID,
                                  false,
                                  &success);

  if (!success)
  {
    // Release resources.
    delete queuePtr;
    delete networkInterfacePtr;
    
    // Exit program.
    return (0);
  } // if

  // Set up for loop entry.
  done = false;

  while (!done)
  {
    // Wait for a commmand.
    success = queuePtr->receiveData(RadioServerTypeCmd,
                                   queueBuffer,
                                   &queueBufferLength);

    if (success)
    {
      // decode the message from the radio client.
      done = decodeMessageQueueCommand(queueBuffer);

      if (!done)
      {
        // Send an ack with no payload.
        success = queuePtr->sendData(RadioServerTypeAck,
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
  delete networkInterfacePtr;
  delete queuePtr;

  return (0);

} // main

