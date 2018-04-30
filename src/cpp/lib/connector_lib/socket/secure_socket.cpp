#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
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
#include "common/aes.h"

#pragma warning(disable:4251)
#include "udt.h"
#pragma warning(default:4251)

#define RSA_DATA_SIZE 2048
#define SSL_HEADER_SIZE	8

std::string SecureSocket::startTls = "STARTTLS";
std::string SecureSocket::expecTls = "EXPECTLS";

SecureSocket::SecureSocket()
	: m_bInit(false)
{
}

SecureSocket::~SecureSocket()
{
}

bool SecureSocket::PerformSslVerify(const std::string& tlsString)
{
    RSA *rsaKey = 0;
    bool bRet = true;
	bool isServer = false;
    try 
	{
		//using variable
		int len = 0;
		unsigned char* data = 0;

		//send STARTTLS to and receive it from other side
		unsigned char sslHeader[SSL_HEADER_SIZE] = {0};
		if (!Send((char*)tlsString.c_str(), SSL_HEADER_SIZE) ||
			!Recv((char*)sslHeader, SSL_HEADER_SIZE))
		{
			throw false;
		}

		if(memcmp(expecTls.c_str(), sslHeader, SSL_HEADER_SIZE) == 0) {
            isServer = true;
            unsigned long bits = RSA_DATA_SIZE;
            rsaKey = RSA_generate_key(bits, RSA_F4, NULL, NULL);

            //send RSA public key
            len = i2d_RSAPublicKey(rsaKey, (unsigned char**)&data);
            if (!Send((char*)&len, sizeof(int)) ||
                !Send((char*)data, len))
            {
                throw false;
            }
            delete data;
        } else if(memcmp(startTls.c_str(), sslHeader, SSL_HEADER_SIZE) == 0) {
            isServer = false;
            rsaKey = RSA_new();
            //receive RSA public key
            if(!Recv((char*)&len, sizeof(int)))
            {
                throw false;
            }
            
            data = new unsigned char[len];
            if (!Recv((char*)data, len) ||
                !d2i_RSAPublicKey(&rsaKey, (const unsigned char**)&data, len))
            {
                throw false;
            }
        }
        else
        {
            throw false;
        }

        len = RSA_size(rsaKey);
        data = new unsigned char[len];

        if(!isServer) {
            //Send session aes key
            if(AESGenerateKey(m_key, sizeof(m_key)) <= 0)
            {
                throw false;
            }
            
            len = RSA_public_encrypt(sizeof(m_key), m_key, data, rsaKey, RSA_PKCS1_PADDING);
            if(!Send((char*)data, len))
            {
                throw false;
            }
            delete data;
        } else {
            data = new unsigned char[len];
            if (!Recv((char*)data, len))
            {
                throw false;
            }
            
            len = RSA_private_decrypt(len, data, m_key, rsaKey, RSA_PKCS1_PADDING);
            delete data;
        
            if (len <= 0)
            {
                throw false;
            }
        }
	}
	catch(bool& ret)
	{
		bRet = ret;
	}

	if(bRet)
	{
		m_bInit = true;
	}

	if(rsaKey)
        RSA_free(rsaKey);
    
	return bRet;
}

bool SecureSocket::Recv(char* data, int len)
{
	int recvlen = 0;
	while(!GetData(data, len))
	{
		if(!RecvData((char*)&recvlen, sizeof(int)))
		{
			return false;
		}

		int realDataLen = GetEncriptedDataLen(recvlen);
		unsigned char* recvdata = new unsigned char[realDataLen];
		if(!RecvData((char*)recvdata, realDataLen))
		{
			return false;
		}
		
		unsigned char* decriptedData = new unsigned char[recvlen];
		int decriptedLen = AESDecrypt(m_key, sizeof(m_key), recvdata, realDataLen, (byte*)decriptedData, recvlen);
		if(decriptedLen == -1)
		{
			return false;
		}

		delete recvdata;
		int newsize = (int)m_recvdata.size() + decriptedLen;
		m_recvdata.resize(newsize, 0);
		memcpy((char*)m_recvdata.c_str() + newsize - decriptedLen, decriptedData, decriptedLen);
		delete decriptedData;
	}
	return true;
}

bool SecureSocket::GetData(char* data, int len)
{
	if((int)m_recvdata.size() < len)
	{
		return false;
	}
	
	memcpy(data, m_recvdata.c_str(), len);
	char *newdata = new char[m_recvdata.size() - len];
	memcpy((char*)newdata, m_recvdata.c_str() + len, m_recvdata.size() - len);
	m_recvdata.resize(m_recvdata.size() - len, 0);
	memcpy((char*)m_recvdata.c_str(), newdata, m_recvdata.size());
	delete newdata;
	return true;
}

bool SecureSocket::Send(char* data, int len)
{
	if(len > MAX_BUFFER_LEN)
	{
		int pos = 0, newlen = MAX_BUFFER_LEN;
		while(pos < len)
		{
			if(!Send(data + pos, newlen))
			{
				return false;
			}
			pos += newlen;
			(len - pos > MAX_BUFFER_LEN) ? (newlen = MAX_BUFFER_LEN) : (newlen = len - pos);
		}
		return true;
	}
	
	unsigned char* encriptedData = new unsigned char[MAX_BUFFER_LEN];
	int sendLen = AESEncrypt(m_key, sizeof(m_key), (byte*)data, len, encriptedData, MAX_BUFFER_LEN);
	if(sendLen <= 0)
	{
		return false;
	}

	unsigned char* senddata = new unsigned char[sendLen + sizeof(int)];
	memcpy(senddata + sizeof(int), encriptedData, sendLen);
	memcpy(senddata, &len, sizeof(int));
	sendLen += sizeof(int);
	delete encriptedData;
	bool bRet = SendData((char*)senddata, sendLen);
	delete senddata;
	return bRet;
}

/*********************************************************************************/
/*                              SecureUDTSocket                                  */
/*********************************************************************************/
SecureUDTSocket::SecureUDTSocket(IPVersion ipv)
	: UDTSocket(ipv)
{
}

SecureUDTSocket::SecureUDTSocket(int socket, IPVersion ipv, bool isUdp)
	: UDTSocket(socket, ipv, isUdp)
{
	if(!isUdp)
	{
		PerformSslVerify(startTls);
	}
}

SecureUDTSocket::SecureUDTSocket(int udpSocket, int socket)
	: UDTSocket(udpSocket, socket)
{
	PerformSslVerify(startTls);
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

	return PerformSslVerify(expecTls);
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
SecureTCPSocket::SecureTCPSocket(IPVersion ipv)
: TCPSocket(ipv)
{
}

SecureTCPSocket::SecureTCPSocket(int socket)
: TCPSocket(socket)
{
	PerformSslVerify(startTls);
}

bool SecureTCPSocket::Connect(const std::string& host, int port)
{
	if(!TCPSocket::Connect(host, port))
	{
		return false;
	}
	
	return PerformSslVerify(expecTls);
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
