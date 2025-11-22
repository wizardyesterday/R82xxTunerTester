//**********************************************************************
// file name: IpcMessageQueue.cc
//**********************************************************************

#include <stdio.h>
#include "IpcMessageQueue.h"

/**************************************************************************

  Name: IpcMessageQueue

  Purpose: The purpose of this function is to serve as the constructor
  of a IpcMessageQueue object.

  Calling Sequence: IpcMessageQueue(fileNamePtr,projectId,
                                    removeQueueInDestructor,
                                    successPtr)

  Inputs:

    fileNamePtr - The name of the file that is used for message key
    generation.

    projectId - An integer that also used for message key generation.
    Only the lowest eight bits are used, and it is common to set this
    parameter to a character value such as the letter 'A'.

    removequeueInDestructor - This is a flag that indicates whether or
    not the message queue is removed in the destructor of the
    IpcMessageQueue object.

    *successPtrPtr - A flag that indicates whether or not the system
    was initialized. A value of true indicates that the systeem was
    initialized, and a value of false indicates that a failure occurred
    during initialization.

  Outputs:

    None.

**************************************************************************/
IpcMessageQueue::IpcMessageQueue(char *fileNamePtr,
    int projectId,
    bool removeQueueInDestructor,
    bool *successPtr)
{

  // Save for later.
  this->removeQueueInDestructor = removeQueueInDestructor;

  //  Defaault to failure result.
  *successPtr = false;
 

  // Generate unique key;
   key = ftok(fileNamePtr,projectId);

  if (key == -1)
  {
    fprintf(stderr,"ERROR: ftok()\n");
  } // if

  if (*successPtr)
  {
    // Create a message queue or use one if it already exists.
    queueIdentifier = msgget(key,IPC_CREAT | 0666);

    if (queueIdentifier == -1)
    {
      fprintf(stderr,"ERROR: msgget()\n");
    } // if

    // The operation worked!
    *successPtr = true;
  } // if

} // IpcMessageQueue

/**************************************************************************

  Name: ~IpcMessageQueue

  Purpose: The purpose of this function is to serve as the destructor
  of a IpcMessageQueue object.

  Calling Sequence: ~IpcMessageQueue()

  Inputs:

    None.

  Outputs:

    None.

**************************************************************************/
IpcMessageQueue::~IpcMessageQueue(void)
{

  if (removeQueueInDestructor)
  {
    if (queueIdentifier != -1)
    {
      // Destroy the queue since we don't want it lingering.
      msgctl(queueIdentifier,IPC_RMID,NULL);
    } // if
  } // if

} // ~IpcMessageQueue

#if 0
/**************************************************************************

  Name: sendData

  Purpose: The purpose of this function is to send a buffer of data over
  a TCP connection.

  Calling Sequence: octetsSent = sendData(bufferPtr,bufferLength)

  Inputs:

    bufferPtr - A pointer to the octets to send.

    bufferLength - The number of octets to send.

  Outputs:

    octetsSent - The number of octets sent over the TCP connection.

**************************************************************************/
ssize_t IpcMessageQueue::sendData(void *bufferPtr,int bufferLength)
{
  ssize_t octetsSent;
  ssize_t octetsWritten;
  ssize_t octetsRemaining;
  unsigned char *octetPtr;

  // Default to nothing sent.
  octetsSent = 0;

  if (connectionIsEstablished())
  {
    // Reference the buffer in the octet context.
    octetPtr = (unsigned char *)bufferPtr;

    // Indicate that the whole buffer needs to be sent.
    octetsRemaining = bufferLength;

    while (octetsRemaining > 0)
    {
      // Send the octets over the TCP connection.
      octetsWritten = send(socketDescriptor,octetPtr,octetsRemaining,0);

      // We havw this much less to send over the network connection.
      octetsRemaining -= octetsWritten;

      // Advance buffer pointer.
      octetPtr += octetsWritten;

      // Update to account for the number of octets written.
      octetsSent += octetsWritten;
    } // while
  } // if

  return (octetsSent);

} // sendData

/**************************************************************************

  Name: receiveData

  Purpose: The purpose of this function is to receive incoming data
  over a TCP connection.

  Calling Sequence: octetsReceived = receiveData(bufferPtr,bufferLength)

  Inputs:

    bufferPtr - A pointer to the octets to send.

    bufferLength - The number of size of the receive buffer, referenced.

  Outputs:

    octetsReceived - The number ogf octects recieved over the connection.


**************************************************************************/
ssize_t  IpcMessageQueue::receiveData(void *bufferPtr,int bufferLength)
{
  ssize_t octetsReceived;
  ssize_t octetsRead;
  bool done;
  unsigned char *octetPtr;
  struct timeval timeout;
  fd_set readFds;

  // Default to nothing received.
  octetsReceived = 0;

  if (connectionIsEstablished())
  {
    // Reference the buffer in the octet context.
    octetPtr = (unsigned char *)bufferPtr;

    // Set up for loop entry.
    done = false;

    while (!done)
    {
      //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
      // This block of code is necessary when working with TCP.
      //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
      // Clear before use.
      FD_ZERO(&readFds);

      FD_SET(socketDescriptor,&readFds);

      // set up timeout value to 200 milliseconds.
      timeout.tv_sec = 0;
      timeout.tv_usec = 200000;

      // wait and be nice to the system
      select(socketDescriptor+1,&readFds,0,0,&timeout);
      //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

      if(FD_ISSET(socketDescriptor,&readFds))
      {
        // Read what's available.
        octetsRead = recv(socketDescriptor,
                               octetPtr,
                               bufferLength,
                               0);

        // Advance to the next storage location.
        octetPtr += octetsRead;

        // Account for new received data.
        octetsReceived += octetsRead;
      } // if
      else
      {
        // All data has been received, so bail out.
        done = true;
      } // else
    } // while
  } // if

  return (octetsReceived);

} // receiveData

#endif
