#ifndef PPP_SOCKET_H
#define PPP_SOCKET_H

#include <string>
#include "any_socket.h"
#include "thread_lib/common/critical_section.h"

class PPPSocket : public AnySocket
{
public:
	explicit PPPSocket(int socket);
	virtual ~PPPSocket();
	
	virtual bool Bind(const std::string& host, int port);
	virtual bool Listen(int limit);
	virtual int Accept(std::string& ip, int& port);
	virtual bool Connect(const std::string& host, int port);
	virtual bool Send(char* data, int len);
	virtual bool Recv(char* data, int len);
	virtual void GetIPPort(std::string& ip, int& port);
	virtual bool Close();
	virtual int GetSocket();
protected:
	int m_socket;
	std::string m_host;
	int m_port;
	CriticalSection m_cs;
};

#endif/*PPP_SOCKET_H*/