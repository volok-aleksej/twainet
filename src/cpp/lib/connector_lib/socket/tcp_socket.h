#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <string>
#include "any_socket.h"
#include "thread_lib/common/critical_section.h"

#include "openssl/ssl.h"
#include "openssl/err.h"

class TCPSocket : public AnySocket
{
public:
	TCPSocket();
	explicit TCPSocket(int socket);
	virtual ~TCPSocket();

	virtual bool Bind(const std::string& host, int port);
	virtual bool Listen(int limit);
	virtual int Accept(std::string& ip, int& port);
	virtual bool Connect(const std::string& host, int port);
	virtual bool Send(char* data, int len);
	virtual bool Recv(char* data, int len);
	virtual void GetIPPort(std::string& ip, int& port);
	virtual bool Close();
	virtual int GetSocket();
private:
	int m_socket;
	CriticalSection m_cs;
};

#endif/*TCP_SOCKET_H*/