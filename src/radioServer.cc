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

// This structure is used to consolidate user parameters.
struct MyParameters
{
  char *serverIpAddressPtr;
  int *serverPortPtr;
};

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// Global variables.
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
TcpClient *networkInterfacePtr;

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// Static functions.
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
static bool keypressed(void);
static void sendRadioCommand(char *commandPtr,uint32_t delayInMilliseconds);

/**************************************************************************

  Name: keypressed

  Purpose: The purpose of this function is to determine of any key
  was pressed on the keyboard.

  Calling Sequence: status = sendRadioCommand(commandPtr,
                                              delayInMilliseconds)
 
  Inputs:

    None.

  Outputs:

    status - An indicator of whether or not a key was pressed. A
    value of true indicates that a key was pressed, and a value of
    false indicates that a key was not pressed.

**************************************************************************/
static bool keypressed(void)
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

} // keypressed

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
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  // Set up for loop entry.
  done = false;

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Retrieve the command line arguments.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  while (!done)
  {
    // Retrieve the next option.
    opt = getopt(argc,argv,"a:p:m:h");

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

      case 'h':
      {
        // Display usage.
        fprintf(stderr,"./tcpClient -a <serverIpAddress: x.x.x.x> "
                "-p <serverPort>\n");

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
  struct MyParameters parameters;
  char *chPtr;

  // Message queue support.
  IpcMessageQueue *queuePtr;
  size_t bufferLength;
  int sendLength;

  // Tcp client support.
  int serverPort;
  char serverIpAddress[32];
  int octetCount;
  char networkOutputBuffer[16384];
  
  // Set up for parameter transmission.
  parameters.serverPortPtr = &serverPort;
  parameters.serverIpAddressPtr = serverIpAddress;

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
  sendRadioCommand("set rxfrequency 57600000\n",0);

  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Radio is set up for testing, so now message queue sruff
  // is set up.
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
  // Instantiate an IPC message queue object.
  queuePtr = new IpcMessageQueue("/home/chris/R82xxKeyFile",
                                 RadioServerCmd,
                                 true, // Change to false later.
                                 &success);

  if (!success)
  {
    // Release resources.
    delete queuePtr;
    delete networkInterfacePtr;
    
    // Exit program.
    return (0);
  } // if
  //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

  // Set up for loop entry.
  done = false;

  while (!done)
  {
    done = keypressed();
  } // while

  // Release resources.
  delete networkInterfacePtr;
  delete queuePtr;

  return (0);

} // main

