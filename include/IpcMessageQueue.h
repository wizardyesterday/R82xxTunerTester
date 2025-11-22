//**********************************************************************
// file name: IpcMessageQueue.h
//**********************************************************************

#ifndef _TCPMESSAGEQUEUE_H_
#define _TCPMESSAGEQUEUE_H_

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// This structure represents a message queue entry.
struct messageBuffer
{
  long mtype;
  char mtext[1024];
};

class IpcMessageQueue
{
  public:

  IpcMessageQueue(char *fileNamePtr,
    int projectId,
    bool removeQueueInDestructor,
    bool *successPtr);

  ~IpcMessageQueue(void);

  bool sendData(void *bufferPtr,int bufferLength,long messageType);
  ssize_t receiveData(void *bufferPtr,int bufferLength);

  private:

  // Attributes
  // This is used for all message queue operations.
  key_t key;
  int queueIdentifier;

  // This is needed since only process should be removing the queue.
  bool removeQueueInDestructor;

  // This is used for message passing.
  messageBuffer sendBuffer;
  messageBuffer receiveBuffer;
};

#endif // _TCPMESSAGEQUEUE_H_
