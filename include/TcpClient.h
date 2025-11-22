//**********************************************************************
// file name: TcpClient.h
//**********************************************************************

#ifndef _TCPCLIENT_H_
#define _TCPCLIENT_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

class TcpClient
{
  public:

  TcpClient(char *ipAddressPtr,int port,bool *successPtrPtr);
  ~TcpClient(void);

  bool connectionIsEstablished(void);
  ssize_t sendData(void *bufferPtr,int bufferLength);
  ssize_t receiveData(void *bufferPtr,int bufferLength);

  private:

  // Attributes
  int socketDescriptor;
  bool connected;
};

#endif // _TCPCLIENT_H_
