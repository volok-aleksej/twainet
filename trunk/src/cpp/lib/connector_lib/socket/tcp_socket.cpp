#include "tcp_socket.h"

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif/*WIN32*/

TCPSocket::TCPSocket(Type typeSocket)
: m_typeSocket(typeSocket), m_ssl(0), m_sslctx(0)
{
	initSSL();
	m_socket = (int)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

TCPSocket::TCPSocket(Type typeSocket, int socket)
: m_typeSocket(typeSocket)
, m_socket(socket)
{
	initSSL();
}

TCPSocket::~TCPSocket()
{
	shutdownSSL();
	Close();
}

bool TCPSocket::Bind(const std::string& host, int port)
{
	if(!m_socket)
	{
		return false;
	}

	sockaddr_in si;
	if(host.empty())
	{
		si.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		hostent* h = gethostbyname(host.c_str());
		if(h)
		{
			si.sin_addr.s_addr = *(u_long*)h->h_addr;
		}
		else
		{
			si.sin_addr.s_addr = inet_addr(host.c_str());
		}
	}

	si.sin_port = htons(port);
	si.sin_family = AF_INET;

	return bind(m_socket, (sockaddr*)&si, sizeof(si)) == 0;
}

bool TCPSocket::Listen(int limit)
{
	if(!m_socket)
	{
		return false;
	}

	return listen(m_socket, limit) == 0;
}

int TCPSocket::Accept(std::string& ip, int& port)
{
	if(!m_socket)
	{
		return INVALID_SOCKET;
	}

	sockaddr_in si;
	int len = sizeof(si);
	int sock = (int)accept(m_socket, (sockaddr*)&si, &len);
	ip = inet_ntoa(si.sin_addr);
	port = si.sin_port;
	return sock;
}

bool TCPSocket::initSSL()
{
	if(m_typeSocket == SECURE_SOCKET)
	{
		m_sslctx = SSL_CTX_new(SSLv3_method());
		if (!m_sslctx)
		{
			return false;
		}

		m_ssl = SSL_new(m_sslctx);
		if (!m_ssl)
		{
			return false;
		}
	}
	else
	{
		m_sslctx = 0;
		m_ssl = 0;
	}
	
	return true;
}

void TCPSocket::shutdownSSL()
{
   if(m_socket > 0 && m_ssl)
   {
      SSL_shutdown(m_ssl);
   }
   
   if (m_sslctx)
   {
      SSL_CTX_free(m_sslctx);
      m_sslctx = NULL;
   }

   if (m_ssl) 
   {
      SSL_free(m_ssl);
      m_ssl = NULL;
   }
}

bool TCPSocket::performSslVerify()
{
	m_typeSocket = SECURE_SOCKET;

	if(!m_ssl)
	{
		initSSL();
	}

	SSL_set_fd(m_ssl, m_socket);
	SSL_set_verify(m_ssl, SSL_VERIFY_NONE, NULL);

	int sslConnectResult = SSL_connect(m_ssl);
	if (1 != sslConnectResult)
	{
		int sslError = SSL_get_error(m_ssl, sslConnectResult);
		return false;
	}

	return true;
}

bool TCPSocket::Connect(const std::string& host, int port)
{
	if(!m_socket || host.empty())
	{
		return false;
	}

	sockaddr_in si;
	hostent* h = gethostbyname(host.c_str());
	if(h)
	{
		si.sin_addr.s_addr = *(u_long*)h->h_addr;
	}
	else
	{
		si.sin_addr.s_addr = inet_addr(host.c_str());
	}

	si.sin_port = htons(port);
	si.sin_family = AF_INET;

	if(connect(m_socket, (sockaddr*)&si, sizeof(si)) != 0)
	{
		return false;
	}
	
	bool sslVerifyRes = true;
	if (SECURE_SOCKET == m_typeSocket)
	{
	   sslVerifyRes = performSslVerify();
	}

	return sslVerifyRes;
}

bool TCPSocket::Send(char* data, int len)
{
	if(!m_socket)
	{
		return false;
	}

	CSLocker locker(&m_cs);
	int sendlen = len;
	while (sendlen > 0)
	{
		int res;
		if(m_typeSocket == SECURE_SOCKET)
		{
			res = SSL_write(m_ssl, data + len - sendlen, sendlen);
		}
		else
		{
			res = send(m_socket, data + len - sendlen, sendlen, 0);
		}
		if(res <= 0)
		{
			return false;
		}

		sendlen -= res;
	}

	return true;
}

bool TCPSocket::Recv(char* data, int len)
{
	if(!m_socket)
	{
		return false;
	}

	int recvlen = len;
	while (recvlen > 0)
	{
		int res;
		if(m_typeSocket == SECURE_SOCKET)
		{
			res = SSL_read(m_ssl, data + len - recvlen, recvlen);
		}
		else
		{
			res = recv(m_socket, data + len - recvlen, recvlen, 0);
		}
		if(res <= 0)
		{
			return false;
		}

		recvlen -= res;
	}

	return true;
}

bool TCPSocket::Close()
{
	if(!m_socket)
	{
		return false;
	}

	shutdown(m_socket, SD_BOTH);
	return closesocket(m_socket) == 0;
}

void TCPSocket::GetIPPort(std::string& ip, int& port)
{
	sockaddr_in addr = {0};
	int len = sizeof(addr);
	if (!getsockname(m_socket, (sockaddr*)&addr, &len))
	{
		ip = inet_ntoa(addr.sin_addr);
		port = ntohs(addr.sin_port);
	}
}

int TCPSocket::GetSocket()
{
	return m_socket;
}