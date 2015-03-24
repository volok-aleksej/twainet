#ifndef SECURE_SOCKET_H
#define SECURE_SOCKET_H

#include <openssl/rsa.h>

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
	unsigned char m_keyOwn[32];
	unsigned char m_keyOther[32];
	RSA* m_rsaOwn;
	RSA* m_rsaOther;
	bool m_bInit;
	unsigned char* m_buffer;
	int m_sizeBuffer;
};

#endif/*SECURE_SOCKET_H*/