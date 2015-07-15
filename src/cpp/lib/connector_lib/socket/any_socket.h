#ifndef ANY_SOCKET_H
#define ANY_SOCKET_H

#include <string>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define SD_BOTH SHUT_RDWR
#endif/*WIN32*/

#ifndef INVALID_SOCKET
#	define INVALID_SOCKET 0
#endif/*INVALID_SOCKET*/
#ifndef SOCKET_ERROR
#	define SOCKET_ERROR -1
#endif/*SOCKET_ERROR*/

class AnySocket
{
public:
	AnySocket(){}
	virtual ~AnySocket(){}
public:
	virtual bool Bind(const std::string& host, int port) = 0;
	virtual bool Listen(int limit) = 0;
	virtual int Accept(std::string& ip, int& port) = 0;
	virtual bool Connect(const std::string& host, int port) = 0;
	virtual bool Send(char* data, int len) = 0;
	virtual bool Recv(char* data, int len) = 0;
	virtual bool Close() = 0;
	virtual void GetIPPort(std::string& ip, int& port) = 0;
	virtual int GetSocket() = 0;
};

#endif/*ANY_SOCKET_H*/