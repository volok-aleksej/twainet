#include "udp_socket.h"

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif/*WIN32*/

UDPSocket::UDPSocket()
{
	m_socket = (int)socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

UDPSocket::UDPSocket(int socket)
: m_socket(socket)
{
}

UDPSocket::~UDPSocket()
{
	Close();
}
	
bool UDPSocket::Bind(const std::string& host, int port)
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

bool UDPSocket::Listen(int limit)
{
	return false;
}

int UDPSocket::Accept(std::string& ip, int& port)
{
	return 0;
}

bool UDPSocket::Connect(const std::string& host, int port)
{
	return false;
}

bool UDPSocket::Send(char* data, int len)
{
	if(!m_socket)
	{
		return false;
	}

	sockaddr_in si;
	if(m_host.empty())
	{
		return false;
	}
	else
	{
		hostent* h = gethostbyname(m_host.c_str());
		if(h)
		{
			si.sin_addr.s_addr = *(u_long*)h->h_addr;
		}
		else
		{
			si.sin_addr.s_addr = inet_addr(m_host.c_str());
		}
	}

	si.sin_port = htons(m_port);
	si.sin_family = AF_INET;

	CSLocker locker(&m_cs);
	int sendlen = len;
	while (sendlen > 0)
	{
		int res = sendto(m_socket, data + len - sendlen, sendlen, 0, (sockaddr*)&si, sizeof(si));
		if(res <= 0)
		{
			return false;
		}

		sendlen -= res;
	}

	return true;
}

bool UDPSocket::Recv(char* data, int len)
{
	if(!m_socket)
	{
		return false;
	}

	int recvlen = len;
	while (recvlen > 0)
	{
		sockaddr_in addr = {0};
		int addrLen = sizeof(addr);
		int res = recvfrom(m_socket, data + len - recvlen, recvlen, 0, (sockaddr*)&addr, &addrLen);
		if(res <= 0)
		{
			return false;
		}

		recvlen -= res;
	}

	return true;
}

void UDPSocket::GetIPPort(std::string& ip, int& port)
{
	sockaddr_in addr = {0};
	int len = sizeof(addr);
	if (!getsockname(m_socket, (sockaddr*)&addr, &len))
	{
		ip = inet_ntoa(addr.sin_addr);
		port = ntohs(addr.sin_port);
	}
}

bool UDPSocket::Close()
{
	if(!m_socket)
	{
		return false;
	}

	shutdown(m_socket, SD_BOTH);
	return closesocket(m_socket) == 0;
}

int UDPSocket::GetSocket()
{
	return m_socket;
}

void UDPSocket::SetSendAddr(const std::string& host, int port)
{
	m_host = host;
	m_port = port;
}

bool UDPSocket::RecvFrom(char* data, int len, std::string& ip, int& port)
{
	if(!m_socket)
	{
		return false;
	}

	int recvlen = len;
	while (recvlen > 0)
	{
		sockaddr_in addr = {0};
		int addrLen = sizeof(addr);
		int res = recvfrom(m_socket, data + len - recvlen, recvlen, 0, (sockaddr*)&addr, &addrLen);
		if(res <= 0)
		{
			return false;
		}

		recvlen -= res;
		
		ip = inet_ntoa(addr.sin_addr);
		port = ntohs(addr.sin_port);
	}

	return true;
}
