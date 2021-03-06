#include "udt_socket.h"

#pragma warning(disable:4251)
#include "udt.h"
#pragma warning(default:4251)

UDTSocket::UDTSocket(IPVersion ipv)
: AnySocket(ipv), m_udpSocket(INVALID_SOCKET)
{
	m_socket = UDT::socket(ipv, SOCK_STREAM, IPPROTO_UDP);
}

UDTSocket::UDTSocket(int socket, IPVersion ipv, bool isUdp)
: AnySocket(ipv), m_udpSocket(INVALID_SOCKET)
{
	if(isUdp)
	{
		m_udpSocket = socket;
		m_socket = UDT::socket(ipv, SOCK_STREAM, IPPROTO_UDP);
	}
	else
	{
		m_socket = socket;
	}
}

UDTSocket::UDTSocket(int udpSocket, int socket)
: m_socket(socket)
, m_udpSocket(udpSocket)
{
	sockaddr_storage si;
#ifdef WIN32
	int len = sizeof(si);
#else
	unsigned int len = sizeof(si);
#endif/*WIN32*/
	getsockname(udpSocket, (sockaddr*)&si, &len);
	m_ipv = (IPVersion)si.ss_family;

}

UDTSocket::~UDTSocket()
{
	Close();
}

bool UDTSocket::Bind(const std::string& host, int port)
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

	if(m_udpSocket == INVALID_SOCKET)
	{
		sockaddr_storage si = {0};
		si.ss_family = m_ipv;
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
				freeaddrinfo(result);
			}
		}

		return UDT::bind(m_socket, (sockaddr*)&si, (m_ipv == IPV4) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6)) == 0;
	}
	else
	{
		int res = UDT::bind(m_socket, m_udpSocket);
		if (res == SOCKET_ERROR)
		{
			return false;
		}

		bool bVal = true;
		if(UDT::setsockopt(m_socket, 0, UDT_RENDEZVOUS, (void*)&bVal, sizeof(bool)) == SOCKET_ERROR)
		{
			return false;
		}

		return true;
	}
}

bool UDTSocket::Listen(int limit)
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

	return UDT::listen(m_socket, limit) == 0;
}

int UDTSocket::Accept(std::string& ip, int& port)
{
	if(m_socket == INVALID_SOCKET)
	{
		return 0;
	}

	sockaddr_storage si;
	int len = sizeof(si);
	int sock = UDT::accept(m_socket, (sockaddr*)&si, &len);
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

bool UDTSocket::Connect(const std::string& host, int port)
{
	if(m_socket == INVALID_SOCKET || host.empty())
	{
		return false;
	}

	sockaddr_storage si;
	si.ss_family = m_ipv;
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
		freeaddrinfo(result);
		//TODO: if ip version 6 and host ip version 4, convert ip4 to ip6
	}

	return UDT::connect(m_socket, (sockaddr*)&si, (m_ipv == IPV4) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6)) == 0;
}

bool UDTSocket::Send(char* data, int len)
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

	CSLocker locker(&m_cs);
	int sendlen = len;
	while (sendlen > 0)
	{
		int res = UDT::send(m_socket, data + len - sendlen, sendlen, 0);
		if(res <= 0)
		{
			return false;
		}

		sendlen -= res;
	}

	return true;
}

bool UDTSocket::Recv(char* data, int len)
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

	int recvlen = len;
	while (recvlen > 0)
	{
		int res = UDT::recv(m_socket, data + len - recvlen, recvlen, 0);
		if(res <= 0)
		{
			return false;
		}

		recvlen -= res;
	}

	return true;
}

bool UDTSocket::Close()
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

	if(m_udpSocket != INVALID_SOCKET)
	{
		shutdown(m_udpSocket, SD_BOTH);
#ifdef WIN32
		closesocket(m_udpSocket);
#else
		close(m_udpSocket);
#endif/*WIN32*/
	}

	return UDT::close(m_socket) == 0;
}

void UDTSocket::GetIPPort(std::string& ip, int& port)
{
	sockaddr_storage addr = {0};
#ifdef WIN32
	int len = sizeof(addr);
#else
	unsigned int len = sizeof(addr);
#endif/*WIN32*/
	if(m_socket != INVALID_SOCKET)
	{
		if ((m_udpSocket != INVALID_SOCKET && !getsockname(m_udpSocket, (sockaddr*)&addr, &len)) ||
			!UDT::getsockname(m_socket, (sockaddr*)&addr, (int*)&len))
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
}

int UDTSocket::GetSocket()
{
	return m_socket;
}

int UDTSocket::GetMaxBufferSize()
{
	if(m_socket == INVALID_SOCKET)
	{
		return 0;
	}
	
	int sndsize, rcvsize;
	int len = sizeof(sndsize);
	if (UDT::getsockopt(m_socket, SOL_SOCKET, UDT_SNDBUF, (char*)&sndsize, &len) != 0 ||
	    UDT::getsockopt(m_socket, SOL_SOCKET, UDT_RCVBUF, (char*)&rcvsize, &len) != 0)
	{
		return 0;
	}
	
	return ((sndsize < rcvsize) ? sndsize : rcvsize);
}