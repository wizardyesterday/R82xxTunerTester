//**********************************************************************
// file name: IpcMessageQueue.h
//**********************************************************************

#ifndef _TCPMESSAGEQUEUE_H_
#define _TCPMESSAGEQUEUE_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

class IpcMessageQueue
{
  public:

  IpcMessageQueue(char *ipAddressPtr,int port,bool *successPtrPtr);
  ~IpcMessageQueue(void);

  bool connectionIsEstablished(void);
  ssize_t sendData(void *bufferPtr,int bufferLength);
  ssize_t receiveData(void *bufferPtr,int bufferLength);

  private:

  // Attributes
  int socketDescriptor;
  bool connected;
};

#endif // _TCPMESSAGEQUEUE_H_
