#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif/*WIN32*/

#include "secure_socket.h"
#include "any_socket.h"

#define RSA_DATA_SIZE 2048

SecureSocket::SecureSocket(AnySocket* socket)
	: m_socket(socket), m_rsaOwn(0), m_rsaOther(0), m_bInit(false)
{
	initSSL();
}

SecureSocket::~SecureSocket()
{
	Shutdown();
}

bool SecureSocket::initSSL()
{
	if(!m_rsaOwn)
	{
		unsigned long bits = RSA_DATA_SIZE;
		m_rsaOwn = RSA_generate_key(bits, RSA_F4, NULL, NULL);
	}
	if(!m_rsaOther)
	{
		m_rsaOther = RSA_new();
	}
	return true;
}

bool SecureSocket::PerformSslVerify()
{	
 	int len = i2d_RSAPublicKey(m_rsaOwn, 0);
	unsigned char* data = 0;
	len = i2d_RSAPublicKey(m_rsaOwn, (unsigned char**)&data);
	if (!Send((char*)&len, sizeof(int)) ||
		!Send((char*)data, len))
	{
		return false;
	}
	delete data;

	if(!Recv((char*)&len, sizeof(int)) ||
		len > RSA_DATA_SIZE)
	{
		return false;
	}
	data = new unsigned char[len];
	if (!Recv((char*)data, len) ||
		!d2i_RSAPublicKey(&m_rsaOther, (const unsigned char**)&data, len))
	{
		return false;
	}

	m_bInit = true;
	return true;
}

bool SecureSocket::Send(char* data, int len)
{
	int sendLen = 0;
	unsigned char* senddata = 0;
	if(m_bInit)
	{
		sendLen = RSA_size(m_rsaOther);
		senddata = new unsigned char[sendLen];
		RSA_public_encrypt(len, (const unsigned char*)data, senddata, m_rsaOther, RSA_PKCS1_PADDING);
	}
	else
	{
		sendLen = len;
		senddata = new unsigned char[sendLen];
		memcpy(senddata, data, len);
	}

	int sendlen = sendLen;
	while (sendlen > 0)
	{
		int res = send(m_socket->GetSocket(), (const char*)(senddata + sendLen - sendlen), sendlen, 0);
		if(res <= 0)
		{
			delete senddata;
			return false;
		}
		
		sendlen -= res;
	};

	delete senddata;

	return true;
}

bool SecureSocket::Recv(char* data, int len)
{
	int recvLen = m_bInit ? RSA_size(m_rsaOwn) : len;
	unsigned char* recvdata = new unsigned char[recvLen];
	
	int recvlen = recvLen;
	while (recvlen > 0)
	{
		int res = recv(m_socket->GetSocket(), (char*)(recvdata + recvLen - recvlen), recvlen, 0);
		if(res <= 0)
		{
			delete recvdata;
			return false;
		}
		
		recvlen -= res;
	}

	if(m_bInit)
	{
		RSA_private_decrypt(RSA_size(m_rsaOwn), recvdata, (unsigned char*)data, m_rsaOwn, RSA_PKCS1_PADDING);
	}
	else
	{
		memcpy(data, recvdata, len);
	}
	
	delete recvdata;

	return true;
}

void SecureSocket::Shutdown()
{
   if(m_rsaOwn)
   {
	   RSA_free(m_rsaOwn);
	   m_rsaOwn = 0;
   }
   if(m_rsaOther)
   {
	   RSA_free(m_rsaOther);
	   m_rsaOther = 0;
   }
}
