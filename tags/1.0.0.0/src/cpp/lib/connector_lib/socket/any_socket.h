#ifndef ANY_SOCKET_H
#define ANY_SOCKET_H

#include <string>
#include "secure_socket.h"

class AnySocket
{
public:
	enum Type
	{
		COMMON_SOCKET = 1,
		SECURE_SOCKET = 2
	};

	AnySocket(Type typeSocket = COMMON_SOCKET)
		: m_typeSocket(typeSocket), m_secure(0)
	{
		if(typeSocket == AnySocket::SECURE_SOCKET)
		{
			m_secure = new SecureSocket(this);
		}
	}
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
protected:
	Type m_typeSocket;
	SecureSocket* m_secure;
};

#endif/*ANY_SOCKET_H*/