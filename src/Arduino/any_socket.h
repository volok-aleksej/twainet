#ifndef ANY_SOCKET_H
#define ANY_SOCKET_H

#include <WString.h>

#include <unistd.h>
#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <netdb.h>
#include <stdio.h>
#include <string.h>
#define SD_BOTH SHUT_RDWR

#ifndef INVALID_SOCKET
#	define INVALID_SOCKET 0
#endif/*INVALID_SOCKET*/

class AnySocket
{
public:
	AnySocket(){}
	virtual ~AnySocket(){}
public:
	virtual bool Bind(const String& host, int port) = 0;
	virtual bool Listen(int limit) = 0;
	virtual int Accept(String& ip, int& port) = 0;
	virtual bool Connect(const String& host, int port) = 0;
	virtual bool Send(char* data, int len) = 0;
	virtual bool Recv(char* data, int len) = 0;
	virtual bool Close() = 0;
	virtual void GetIPPort(String& ip, int& port) = 0;
	virtual int GetSocket() = 0;
	virtual int GetMaxBufferSize() = 0;
};

#endif/*ANY_SOCKET_H*/