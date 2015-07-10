#ifndef SECURE_SOCKET_H
#define SECURE_SOCKET_H

#include <openssl/rsa.h>
#include "udt_socket.h"
#include "tcp_socket.h"
#include "ppp_socket.h"

class AnySocket;

class SecureSocket
{
public:
	SecureSocket();
	virtual ~SecureSocket();

	virtual bool Send(char* data, int len);
	virtual bool Recv(char* data, int len);
protected:
	bool PerformSslVerify();

	virtual bool SendData(char* data, int len) = 0;
	virtual bool RecvData(char* data, int len) = 0;

	bool GetData(char* data, int len);
protected:
	bool m_bInit;
	unsigned char m_keyOwn[32];
	unsigned char m_keyOther[32];
	std::string m_recvdata;
public:
	static char m_ssl_header[];
};


class SecureUDTSocket : public SecureSocket, public UDTSocket
{
public:
	SecureUDTSocket();
	explicit SecureUDTSocket(int socket, bool isUdp);
	explicit SecureUDTSocket(int udpSocket, int socket);

	virtual bool Connect(const std::string& host, int port);
	virtual bool Send(char* data, int len);
	virtual bool Recv(char* data, int len);
protected:
	virtual bool SendData(char* data, int len);
	virtual bool RecvData(char* data, int len);
};

class SecureTCPSocket : public SecureSocket, public TCPSocket
{
public:
	SecureTCPSocket();
	explicit SecureTCPSocket(int socket);

	virtual bool Connect(const std::string& host, int port);
	virtual bool Send(char* data, int len);
	virtual bool Recv(char* data, int len);
protected:
	virtual bool SendData(char* data, int len);
	virtual bool RecvData(char* data, int len);
};

#ifdef WIN32
class SecurePPPSocket : public SecureSocket, public PPPSocket
{
public:
	explicit SecurePPPSocket(int socket);

	virtual bool Connect(const std::string& host, int port);
	virtual bool Send(char* data, int len);
	virtual bool Recv(char* data, int len);
protected:
	virtual bool SendData(char* data, int len);
	virtual bool RecvData(char* data, int len);
};

#endif/*WIN32*/

#endif/*SECURE_SOCKET_H*/