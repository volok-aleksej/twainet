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
#include "udt_socket.h"
#include "common\aes.h"

#pragma warning(disable:4251)
#include "udt.h"
#pragma warning(default:4251)

#define RSA_DATA_SIZE 2048
#define SSL_HEADER_SIZE	8
#define SSL_HEADER	"STARTTLS"

SecureSocket::SecureSocket()
	: m_bInit(false)
{
}

SecureSocket::~SecureSocket()
{
}

bool SecureSocket::PerformSslVerify()
{
	unsigned long bits = RSA_DATA_SIZE;
	RSA* rsaOwn = RSA_generate_key(bits, RSA_F4, NULL, NULL);
	RSA* rsaOther = RSA_new();
	bool bRet = true;

	try 
	{
		//using variable
		int len = 0;
		unsigned char* data = 0;

		//send STARTTLS to and receive it from other side
		unsigned char sslHeader[SSL_HEADER_SIZE] = {0};
		if (!Send(SSL_HEADER, SSL_HEADER_SIZE) ||
			!Recv((char*)sslHeader, SSL_HEADER_SIZE) ||
			memcmp(sslHeader, SSL_HEADER, SSL_HEADER_SIZE) != 0)
		{
			throw false;
		}

		//send RSA public key
		len = i2d_RSAPublicKey(rsaOwn, (unsigned char**)&data);
		if (!Send((char*)&len, sizeof(int)) ||
			!Send((char*)data, len))
		{
			throw false;
		}
		delete data;

		//receive RSA public key
		if(!Recv((char*)&len, sizeof(int)))
		{
			throw false;
		}
		data = new unsigned char[len];
		if (!Recv((char*)data, len) ||
			!d2i_RSAPublicKey(&rsaOther, (const unsigned char**)&data, len))
		{
			throw false;
		}

		//Send session aes key
		if(AESGenerateKey(m_keyOwn, sizeof(m_keyOwn)) <= 0)
		{
			throw false;
		}
		len = RSA_size(rsaOther);
		data = new unsigned char[len];
		len = RSA_public_encrypt(sizeof(m_keyOwn), m_keyOwn, data, rsaOther, RSA_PKCS1_PADDING);
		if(!Send((char*)data, len))
		{
			throw false;
		}
		delete data;

		len = RSA_size(rsaOwn);
		data = new unsigned char[len];
		if (!Recv((char*)data, len) ||
			RSA_private_decrypt(len, data, m_keyOther, rsaOwn, RSA_PKCS1_PADDING) <= 0)
		{
			throw false;
		}
		delete data;
	}
	catch(bool& ret)
	{
		bRet = ret;
	}

	if(bRet)
	{
		m_bInit = true;
	}

	RSA_free(rsaOwn);
	RSA_free(rsaOther);
	return true;
}

bool SecureSocket::Recv(char* data, int len)
{
	int recvlen = 0;
	if(!RecvData((char*)&recvlen, sizeof(int)))
	{
		return false;
	}

	unsigned char* recvdata = new unsigned char[recvlen];
	if(!RecvData((char*)recvdata, recvlen))
	{
		return false;
	}

	int decriptedLen = AESDecrypt(m_keyOwn, sizeof(m_keyOwn), recvdata, recvlen, (byte*)data, len);
	return decriptedLen == len;
}

bool SecureSocket::Send(char* data, int len)
{
	unsigned char* encriptedData = new unsigned char[MAX_DATA_LEN];
	int sendLen = AESEncrypt(m_keyOther, sizeof(m_keyOther), (byte*)data, len, encriptedData, MAX_DATA_LEN);
	if(sendLen <= 0)
	{
		return false;
	}

	unsigned char* senddata = new unsigned char[sendLen + sizeof(int)];
	memcpy(senddata + sizeof(int), encriptedData, sendLen);
	memcpy(senddata, &sendLen, sizeof(int));
	sendLen += sizeof(int);
	delete encriptedData;
	bool bRet = SendData((char*)senddata, sendLen);
	delete senddata;
	return bRet;
}

/*********************************************************************************/
/*                              SecureUDTSocket                                  */
/*********************************************************************************/
SecureUDTSocket::SecureUDTSocket()
{
}

SecureUDTSocket::SecureUDTSocket(int socket, bool isUdp)
	: UDTSocket(socket, isUdp)
{
}

SecureUDTSocket::SecureUDTSocket(int udpSocket, int socket)
	: UDTSocket(udpSocket, socket)
{
	PerformSslVerify();
}

bool SecureUDTSocket::Send(char* data, int len)
{
	if(m_bInit)
	{
		return SecureSocket::Send(data, len);
	}
	else
	{
		return SendData(data, len);
	}
}

bool SecureUDTSocket::Recv(char* data, int len)
{
	if(m_bInit)
	{
		return SecureSocket::Recv(data, len);
	}
	else
	{
		return RecvData(data, len);
	}
}

bool SecureUDTSocket::Connect(const std::string& host, int port)
{
	if(!UDTSocket::Connect(host, port))
	{
		return false;
	}

	return PerformSslVerify();
}

bool SecureUDTSocket::SendData(char* data, int len)
{
	return UDTSocket::Send(data, len);
}

bool SecureUDTSocket::RecvData(char* data, int len)
{
	return UDTSocket::Recv(data, len);
}


/*********************************************************************************/
/*                              SecureTCPSocket                                  */
/*********************************************************************************/
SecureTCPSocket::SecureTCPSocket()
{
}

SecureTCPSocket::SecureTCPSocket(int socket)
: TCPSocket(socket)
{
	PerformSslVerify();
}

bool SecureTCPSocket::Connect(const std::string& host, int port)
{
	if(!TCPSocket::Connect(host, port))
	{
		return false;
	}
	
	return PerformSslVerify();
}

bool SecureTCPSocket::Send(char* data, int len)
{
	if(m_bInit)
	{
		return SecureSocket::Send(data, len);
	}
	else
	{
		return SendData(data, len);
	}
}

bool SecureTCPSocket::Recv(char* data, int len)
{
	if(m_bInit)
	{
		return SecureSocket::Recv(data, len);
	}
	else
	{
		return RecvData(data, len);
	}
}

bool SecureTCPSocket::SendData(char* data, int len)
{
	return TCPSocket::Send(data, len);
}

bool SecureTCPSocket::RecvData(char* data, int len)
{
	return TCPSocket::Recv(data, len);
}