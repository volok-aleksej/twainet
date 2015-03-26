#ifndef UDT_SOCKET_H
#define UDT_SOCKET_H

#include "any_socket.h"
#include "thread_lib/common/critical_section.h"

class UDTSocket : public AnySocket
{
public:
	UDTSocket();
	explicit UDTSocket(int socket, bool isUdp);
	explicit UDTSocket(int udpSocket, int socket);
	virtual ~UDTSocket();
	
	virtual bool Bind(const std::string& host, int port);
	virtual bool Listen(int limit);
	virtual int Accept(std::string& ip, int& port);
	virtual bool Connect(const std::string& host, int port);
	virtual bool Send(char* data, int len);
	virtual bool Recv(char* data, int len);
	virtual bool Close();
	virtual void GetIPPort(std::string& ip, int& port);
	virtual int GetSocket();
private:
	int m_udpSocket;
	int m_socket;

	CriticalSection m_cs;
};

#endif/*UDT_SOCKET_H*/