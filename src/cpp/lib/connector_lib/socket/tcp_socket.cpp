#include "tcp_socket.h"

TCPSocket::TCPSocket(IPVersion ipv)
: AnySocket(ipv)
{
	Initialize();
}

TCPSocket::TCPSocket(int socket)
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

TCPSocket::~TCPSocket()
{
	Close();
}

bool TCPSocket::Bind(const std::string& host, int port)
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

	sockaddr_storage si = {0};
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

bool TCPSocket::Listen(int limit)
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

	return listen(m_socket, limit) == 0;
}

int TCPSocket::Accept(std::string& ip, int& port)
{
	if(m_socket == INVALID_SOCKET)
	{
		return INVALID_SOCKET;
	}

	sockaddr_storage si;
#ifdef WIN32
	int len = (m_ipv == IPV4) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6);
#else
	unsigned int len = (m_ipv == IPV4) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6);
#endif/*WIN32*/
	int sock = (int)accept(m_socket, (sockaddr*)&si, &len);
	size_t lenaddr = 0;
	void* paddr = 0;
	if(m_ipv == IPV4)
	{
		paddr = &((sockaddr_in&)si).sin_addr;
		len = INET_ADDRSTRLEN;
		port = ntohs(((sockaddr_in&)si).sin_port);
	}
	else
	{
		paddr = &((sockaddr_in6&)si).sin6_addr;
		len = INET6_ADDRSTRLEN;
		port = ntohs(((sockaddr_in6&)si).sin6_port);
	}

	char *ipinput = new char[len];
	inet_ntop(m_ipv, paddr, ipinput, lenaddr);
	ip = ipinput;
	delete ipinput;
	return sock;
}

bool TCPSocket::Connect(const std::string& host, int port)
{
	if(m_socket == INVALID_SOCKET || host.empty())
	{
		return false;
	}

	sockaddr_storage si;
	char portstr[10] = {0};
#ifdef WIN32
	_itoa_s(port, portstr, 10, 10);
#else
	sprintf(portstr, "%d", port);
#endif/*WIN32*/
	addrinfo hints = {0}, *result;
	hints.ai_family = AF_UNSPEC;
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
		//TODO: if ip version 6 and host ip version 4, convert ip4 to ip6
	}
	freeaddrinfo(result);

	return connect(m_socket, (sockaddr*)&si, (m_ipv == IPV4) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6)) == 0;
}

bool TCPSocket::Send(char* data, int len)
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

	CSLocker locker(&m_cs);
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

	return true;
}

bool TCPSocket::Recv(char* data, int len)
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

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
	
	return true;
}

bool TCPSocket::Close()
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

void TCPSocket::GetIPPort(std::string& ip, int& port)
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

int TCPSocket::GetSocket()
{
	return m_socket;
}

void TCPSocket::Initialize()
{
	m_socket = (int)socket(m_ipv, SOCK_STREAM, IPPROTO_TCP);
	int opt = 0;
#ifdef WIN32
	int len = sizeof(opt);
#else
	socklen_t len = sizeof(opt);
#endif/*WIN32*/
	getsockopt(m_socket, SOL_SOCKET, IPV6_V6ONLY, (char*)&opt, &len);
}

int TCPSocket::GetMaxBufferSize()
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
