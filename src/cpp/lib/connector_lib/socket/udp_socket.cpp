#include "udp_socket.h"
#include <string.h>

UDPSocket::UDPSocket(IPVersion ipv)
: AnySocket(ipv)
{
	m_socket = (int)socket(ipv, SOCK_DGRAM, IPPROTO_UDP);
}

UDPSocket::UDPSocket(int socket)
: m_socket(socket)
{
	sockaddr_storage si;
#ifdef WIN32
	int len = sizeof(si);
#else
	unsigned int len = sizeof(si);
#endif/*WIN32*/
	getsockname(m_socket, (sockaddr*)&si, &len);
	m_ipv = (IPVersion)si.ss_family;
}

UDPSocket::~UDPSocket()
{
	Close();
}
	
bool UDPSocket::Bind(const std::string& host, int port)
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

	sockaddr_storage si;
	if(host.empty())
	{
		if(m_ipv == IPV4)
			((sockaddr_in&)si).sin_addr.s_addr = INADDR_ANY;
		else
			((sockaddr_in6&)si).sin6_addr = in6addr_any;
	}
	else
	{
		char portstr[10] = {0};
#ifdef WIN32
		_itoa_s(port, portstr, 10, 10);
#else
		sprintf(portstr, "%d", port);
#endif/*WIN32*/
		addrinfo hints = {0}, *result;
		hints.ai_family = m_ipv;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		int ret = getaddrinfo(host.c_str(), portstr, &hints, &result);
		if(!ret)
		{
			for(addrinfo *addr = result; addr != 0; addr = addr->ai_next)
			{
				if(addr->ai_family == m_ipv)
				{
					memcpy(&si, addr->ai_addr, addr->ai_addrlen);
				}
			}
		}
		freeaddrinfo(result);
	}

	return bind(m_socket, (sockaddr*)&si, m_ipv == IPV4 ? sizeof(sockaddr_in) : sizeof(sockaddr_in6)) == 0;
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
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

	sockaddr_storage si;
	if(m_host.empty())
	{
		return false;
	}
	else
	{
		char portstr[10] = {0};
#ifdef WIN32
		_itoa_s(m_port, portstr, 10, 10);
#else
		sprintf(portstr, "%d", m_port);
#endif/*WIN32*/
		addrinfo hints = {0}, *result;
		hints.ai_family = m_ipv;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		int ret = getaddrinfo(m_host.c_str(), portstr, &hints, &result);
		if(!ret)
		{
			for(addrinfo *addr = result; addr != 0; addr = addr->ai_next)
			{
				if(addr->ai_family == m_ipv)
				{
					memcpy(&si, addr->ai_addr, addr->ai_addrlen);
				}
			}
		}
		freeaddrinfo(result);
	}

	CSLocker locker(&m_cs);
	int sendlen = len;
	while (sendlen > 0)
	{
		int res = sendto(m_socket, data + len - sendlen, sendlen, 0, (sockaddr*)&si, (m_ipv == IPV4) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6));
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
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

	int recvlen = len;
	while (recvlen > 0)
	{
		sockaddr_storage addr = {0};
#ifdef WIN32
		int addrLen = sizeof(addr);
#else
		unsigned int addrLen = sizeof(addr);
#endif/*WIN32*/
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
	sockaddr_storage addr = {0};
#ifdef WIN32
		int len = (m_ipv == IPV4) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6);
#else
		unsigned int len = (m_ipv == IPV4) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6);
#endif/*WIN32*/
	if (!getsockname(m_socket, (sockaddr*)&addr, &len))
	{
		size_t len = 0;
		void* paddr = 0;
		if(m_ipv == IPV4)
		{
			paddr = &((sockaddr_in&)addr).sin_addr;
			len = INET_ADDRSTRLEN;
			port = ntohs(((sockaddr_in&)addr).sin_port);
		}
		else
		{
			paddr = &((sockaddr_in6&)addr).sin6_addr;
			len = INET6_ADDRSTRLEN;
			port = ntohs(((sockaddr_in6&)addr).sin6_port);
		}

		char *ipinput = new char[len];
		inet_ntop(m_ipv, paddr, ipinput, len);
		ip = ipinput;
		delete ipinput;
	}
}

bool UDPSocket::Close()
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

	shutdown(m_socket, SD_BOTH);
#ifdef WIN32
	return closesocket(m_socket) == 0;
#else
	return close(m_socket) == 0;
#endif/*WIN32*/
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
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

	unsigned int recvlen = len;
	while (recvlen > 0)
	{
		sockaddr_storage addr = {0};
#ifdef WIN32
		int addrLen = sizeof(addr);
#else
		unsigned int addrLen = sizeof(addr);
#endif/*WIN32*/
		int res = recvfrom(m_socket, data + len - recvlen, recvlen, 0, (sockaddr*)&addr, &addrLen);
		if(res <= 0)
		{
			return false;
		}

		recvlen -= res;
		
		size_t lenaddr = 0;
		void* paddr = 0;
		if(m_ipv == IPV4)
		{
			paddr = &((sockaddr_in&)addr).sin_addr;
			lenaddr = INET_ADDRSTRLEN;
			port = ntohs(((sockaddr_in&)addr).sin_port);
		}
		else
		{
			paddr = &((sockaddr_in6&)addr).sin6_addr;
			lenaddr = INET6_ADDRSTRLEN;
			port = ntohs(((sockaddr_in6&)addr).sin6_port);
		}

		char *ipinput = new char[lenaddr];
		inet_ntop(m_ipv, paddr, ipinput, lenaddr);
		ip = ipinput;
		delete ipinput;
	}

	return true;
}

int UDPSocket::GetMaxBufferSize()
{
	if(m_socket == INVALID_SOCKET)
	{
		return 0;
	}
	
	int sndsize, rcvsize;
#ifdef WIN32
	int len = sizeof(sndsize);
#else
	socklen_t len = sizeof(sndsize);
#endif/*WIN32*/
	if (getsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (char*)&sndsize, &len) != 0 ||
	    getsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*)&rcvsize, &len) != 0)
	{
		return 0;
	}
	
	return ((sndsize < rcvsize) ? sndsize : rcvsize);
}