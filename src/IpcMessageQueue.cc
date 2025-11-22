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

    *successPtrPtr - A pointer to flag that indicates whether or not the
    system was initialized. A value of true indicates that the systeem was
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

/**************************************************************************

  Name: sendData

  Purpose: The purpose of this function is to send a buffer of data to
  another process via a message queue.

  Calling Sequence: success = sendData(messageType,bufferPtr,bufferLength)

  Inputs:

    messageType - The type of message that is used for multiplexing of
    messages on the message queue.

    bufferPtr - A pointer to the octets to send.

    bufferLength - The number of octets to send.

  Outputs:

    success - A flag that indicates whether or not the operation was
    successful. A value of true indicates that the operation was
    successful, and a value of false indicates that a failure occurred.


**************************************************************************/
bool IpcMessageQueue::sendData(
  long messageType,
  void *bufferPtr,
  size_t bufferLength)
{
  bool success;
  int result;

  // Default to failure.
  success = false;
  // Set up a message.
  sendBuffer.mtype =messageType;
  memcpy(sendBuffer.mtext,bufferPtr,bufferLength);

  // Send the message.
  result = msgsnd(queueIdentifier,&sendBuffer,bufferLength,0);

  if (result == 0)
  {
    // The message was succwssfully queued.
    success = true;
  } // if


  return (success);

} // sendData

/**************************************************************************

  Name: receiveData

  Purpose: The purpose of this function is to receive incoming data
  sent from another process via message queues.

  Calling Sequence: success = receiveData(messageType,bufferPtr,bufferLengthPtr)

  Inputs:

    messageType - The type of message that is used for multiplexing of
    messages on the message queue.

    bufferPtr - A pointer to storage to accept received data.

    bufferLengthPtr - A pointer to storage that indicates how many
    message bytes were received.

  Outputs:

    success - A flag that indicates whether or not the operation was
    successful. A value of true indicates that the operation was
    successful, and a value of false indicates that a failure occurred.

**************************************************************************/
bool IpcMessageQueue::receiveData(
  long messageType,
  void *bufferPtr,
  size_t *bufferLengthPtr)
{
  bool success;

  // Default ro failure.
  success = false;

  // Wait for response.
  *bufferLengthPtr = msgrcv(queueIdentifier,
                            &receiveBuffer,
                            sizeof(receiveBuffer.mtext),
                            messageType,
                            0);

  if (*bufferLengthPtr != 0)
  {
    // Copy the message data to the supplied storage.
    memcpy(sendBuffer.mtext,bufferPtr,*bufferLengthPtr);

    // We succeeded!
    success = true;
  } // if

  return (success);

} // receiveData
