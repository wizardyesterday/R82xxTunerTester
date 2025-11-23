#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "TcpClient.h"
#include "R82xxTunerTester.h"

#define DEFAULT_SERVER_IP_ADDRESS "192.93.16.87"
#define DEFAULT_SERVER_PORT (8001)

// This structure represents an entry in a message queue.
struct messageBuffer
{
  long mtype;
  char mtext[1024];
};

// This structure is used to consolidate user parameters.
struct MyParameters
{
  char *serverIpAddressPtr;
  int *serverPortPtr;
};

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
  size_t bufferLength;
  int sendLength;

  // Tcp client support.
  int octetCount;
  char *networkBufferPtr;
  char inputBuffer[16384];
  char networkInputBuffer[16384];
  char networkOutputBuffer[16384];
  int serverPort;
  char serverIpAddress[32];
  TcpClient *networkInterfacePtr;
  
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

  networkBufferPtr = "enable iqdump\n";
  octetCount = strlen(networkBufferPtr);
  octetCount = networkInterfacePtr->sendData(networkBufferPtr,octetCount);
  octetCount = networkInterfacePtr->receiveData(networkOutputBuffer,16000);
  networkOutputBuffer[octetCount] = 0;
  printf("%s",networkOutputBuffer);

  networkBufferPtr = "set demodmode 0\n";
  octetCount = strlen(networkBufferPtr);
  octetCount = networkInterfacePtr->sendData(networkBufferPtr,octetCount);
  octetCount = networkInterfacePtr->receiveData(networkOutputBuffer,16000);
  networkOutputBuffer[octetCount] = 0;
  printf("%s",networkOutputBuffer);

  networkBufferPtr = "set agclevel -8\n";
  octetCount = strlen(networkBufferPtr);
  octetCount = networkInterfacePtr->sendData(networkBufferPtr,octetCount);
  octetCount = networkInterfacePtr->receiveData(networkOutputBuffer,16000);
  printf("%s",networkOutputBuffer);

  networkBufferPtr = "set rxifgain 0\n";
  octetCount = strlen(networkBufferPtr);
  octetCount = networkInterfacePtr->sendData(networkBufferPtr,octetCount);
  octetCount = networkInterfacePtr->receiveData(networkOutputBuffer,16000);
  printf("%s",networkOutputBuffer);

  networkBufferPtr = "set rxgain 50\n";
  octetCount = strlen(networkBufferPtr);
  octetCount = networkInterfacePtr->sendData(networkBufferPtr,octetCount);
  octetCount = networkInterfacePtr->receiveData(networkOutputBuffer,16000);
  printf("%s",networkOutputBuffer);

  networkBufferPtr = "start receiver\n";
  octetCount = strlen(networkBufferPtr);
  octetCount = networkInterfacePtr->sendData(networkBufferPtr,octetCount);
  usleep(5000000);
  octetCount = networkInterfacePtr->receiveData(networkOutputBuffer,16000);
  printf("%s",networkOutputBuffer);

  networkBufferPtr = "start ringoscillator 8 48 0\n";
  octetCount = strlen(networkBufferPtr);
  octetCount = networkInterfacePtr->sendData(networkBufferPtr,octetCount);
  octetCount = networkInterfacePtr->receiveData(networkOutputBuffer,16000);
  printf("%s",networkOutputBuffer);

  networkBufferPtr = "set rxfrequency 57600000\n";
  octetCount = strlen(networkBufferPtr);
  octetCount = networkInterfacePtr->sendData(networkBufferPtr,octetCount);
  octetCount = networkInterfacePtr->receiveData(networkOutputBuffer,16000);
  printf("%s",networkOutputBuffer);

  // Release resources.
  delete networkInterfacePtr;

  return (0);

} // main
