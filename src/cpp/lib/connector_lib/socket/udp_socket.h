#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H

#include <string>
#include "any_socket.h"
#include "thread_lib/common/critical_section.h"

class UDPSocket : public AnySocket
{
public:
	UDPSocket(IPVersion ipv);
	explicit UDPSocket(int socket);
	virtual ~UDPSocket();
	
	virtual bool Bind(const std::string& host, int port);
	virtual bool Listen(int limit);
	virtual int Accept(std::string& ip, int& port);
	virtual bool Connect(const std::string& host, int port);
	virtual bool Send(char* data, int len);
	virtual bool Recv(char* data, int len);
	virtual void GetIPPort(std::string& ip, int& port);
	virtual bool Close();
	virtual int GetSocket();
	virtual int GetMaxBufferSize();

	bool RecvFrom(char* data, int len, std::string& ip, int& port);
	void SetSendAddr(const std::string& host, int port);
protected:
	int m_socket;
	std::string m_host;
	int m_port;
	CriticalSection m_cs;
};

#endif/*UDP_SOCKET_H*/