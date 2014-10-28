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
	: AnySocket(typeSocket)
{
	m_socket = (int)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

TCPSocket::TCPSocket(Type typeSocket, int socket)
: AnySocket(typeSocket)
, m_socket(socket)
{
	if (SECURE_SOCKET == m_typeSocket)
	{
	   m_secure->PerformSslVerify();
	}
}

TCPSocket::~TCPSocket()
{
	if(m_typeSocket == AnySocket::SECURE_SOCKET)
	{
		m_secure->Shutdown();
		delete m_secure;
	}
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
	   sslVerifyRes = m_secure->PerformSslVerify();
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
	if(m_typeSocket == SECURE_SOCKET)
	{
		return m_secure->Send(data, len);
	}
	else
	{
		int sendlen = len;
		while (sendlen > 0)
		{
			int res = send(m_socket, data + len - sendlen, sendlen, 0);
			if(res <= 0)
			{
				return false;
			}

			sendlen -= res;
		}
	}

	return true;
}

bool TCPSocket::Recv(char* data, int len)
{
	if(!m_socket)
	{
		return false;
	}

	if(m_typeSocket == SECURE_SOCKET)
	{
		return m_secure->Recv(data, len);
	}
	else
	{
		int recvlen = len;
		while (recvlen > 0)
		{
			int res = recv(m_socket, data + len - recvlen, recvlen, 0);
			if(res <= 0)
			{
				return false;
			}

			recvlen -= res;
		}
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
