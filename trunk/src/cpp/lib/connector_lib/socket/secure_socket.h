#ifndef SECURE_SOCKET_H
#define SECURE_SOCKET_H

#include "openssl/ssl.h"
#include "openssl/err.h"

class AnySocket;

class SecureSocket
{
public:
	SecureSocket(AnySocket* socket);
	virtual ~SecureSocket();

	bool PerformSslVerify();
	bool Send(char* data, int len);
	bool Recv(char* data, int len);
	void Shutdown();
protected:
	bool initSSL();
private:
	AnySocket* m_socket;
	SSL_CTX* m_sslctx;
	SSL* m_ssl;
};

#endif/*SECURE_SOCKET_H*/