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
	enum Type
	{
		COMMON_SOCKET = 1,
		SECURE_SOCKET = 2
	};

	TCPSocket(Type typeSocket);
	explicit TCPSocket(Type typeSocket, int socket);
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
protected:
	bool initSSL();
	bool performSslVerify();
	void shutdownSSL();
private:
	int m_socket;
	CriticalSection m_cs;
	Type m_typeSocket;

	SSL_CTX* m_sslctx;
	SSL* m_ssl;
};

#endif/*TCP_SOCKET_H*/