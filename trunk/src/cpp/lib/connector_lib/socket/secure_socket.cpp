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
#include "common\aes.h"

#define RSA_DATA_SIZE 2048
#define SSL_HEADER_SIZE	8
#define SSL_HEADER	"STARTTLS"

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
	//send STARTTLS to and receive it from other side
	unsigned char sslHeader[SSL_HEADER_SIZE] = {0};
	if (!Send(SSL_HEADER, SSL_HEADER_SIZE) ||
		!Recv((char*)sslHeader, SSL_HEADER_SIZE) ||
		memcmp(sslHeader, SSL_HEADER, SSL_HEADER_SIZE) != 0)
	{
		return false;
	}

	//send RSA public key
 	int len = i2d_RSAPublicKey(m_rsaOwn, 0);
	unsigned char* data = 0;
	len = i2d_RSAPublicKey(m_rsaOwn, (unsigned char**)&data);
	if (!Send((char*)&len, sizeof(int)) ||
		!Send((char*)data, len))
	{
		return false;
	}
	delete data;

	//receive RSA public key
	if(!Recv((char*)&len, sizeof(int)))
	{
		return false;
	}
	data = new unsigned char[len];
	if (!Recv((char*)data, len) ||
		!d2i_RSAPublicKey(&m_rsaOther, (const unsigned char**)&data, len))
	{
		return false;
	}

	//Send session aes key
	if(AESGenerateKey(m_keyOwn, sizeof(m_keyOwn)) <= 0)
	{
		return false;
	}
	len = RSA_size(m_rsaOther);
	data = new unsigned char[len];
	len = RSA_public_encrypt(sizeof(m_keyOwn), m_keyOwn, data, m_rsaOther, RSA_PKCS1_PADDING);
	if(!Send((char*)data, len))
	{
		return false;
	}
	delete data;

	len = RSA_size(m_rsaOwn);
	data = new unsigned char[len];
	if (!Recv((char*)data, len) ||
		RSA_private_decrypt(len, data, m_keyOther, m_rsaOwn, RSA_PKCS1_PADDING) <= 0)
	{
		return false;
	}
	delete data;

	m_bInit = true;
	return true;
}

bool SecureSocket::Send(char* data, int len)
{
	int sendLen = 0;
	unsigned char* senddata = 0;
	if(m_bInit)
	{
		unsigned char* encriptedData = new unsigned char[MAX_DATA_LEN];
		sendLen = AESEncrypt(m_keyOther, sizeof(m_keyOther), (byte*)data, len, encriptedData, MAX_DATA_LEN);
		if(sendLen <= 0)
		{
			return false;
		}

		senddata = new unsigned char[sendLen + sizeof(int)];
		memcpy(senddata + sizeof(int), encriptedData, sendLen);
		memcpy(senddata, &sendLen, sizeof(int));
		sendLen += sizeof(int);
		delete encriptedData;
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
	int recvLen = m_bInit ? sizeof(int) : len;
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
		if(m_bInit && recvLen == sizeof(int) && recvlen == 0)
		{
			recvLen = *(int*)recvdata;
			recvlen = recvLen;
			delete recvdata;
			recvdata = new unsigned char[recvLen];
		}
	}

	if(m_bInit)
	{
		int decriptedLen = AESDecrypt(m_keyOwn, sizeof(m_keyOwn), recvdata, recvLen, (byte*)data, len);
		if(decriptedLen != len)
		{
			return false;
		}
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
