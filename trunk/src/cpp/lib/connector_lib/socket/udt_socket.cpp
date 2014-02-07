#include "udt_socket.h"

#pragma warning(disable:4251)
#include "udt.h"
#pragma warning(default:4251)

UDTSocket::UDTSocket()
: m_udpSocket(0)
{
	m_socket = UDT::socket(AF_INET, SOCK_STREAM, IPPROTO_UDP);
}

UDTSocket::UDTSocket(int socket, bool isUdp)
: m_udpSocket(0)
{
	if(isUdp)
	{
		m_udpSocket = socket;
		m_socket = UDT::socket(AF_INET, SOCK_STREAM, IPPROTO_UDP);
	}
	else
	{
		m_socket = socket;
	}
}

UDTSocket::UDTSocket(int udpSocket, int socket)
{
	m_udpSocket = udpSocket;
	m_socket = socket;
}

UDTSocket::~UDTSocket()
{
}

bool UDTSocket::Bind(const std::string& host, int port)
{
	if(!m_socket)
	{
		return false;
	}

	if(!m_udpSocket)
	{
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
		return UDT::bind(m_socket, (sockaddr*)&si, sizeof(si)) == 0;
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
	if(!m_socket)
	{
		return false;
	}

	return UDT::listen(m_socket, limit) == 0;
}

int UDTSocket::Accept(std::string& ip, int& port)
{
	if(!m_socket)
	{
		return 0;
	}

	sockaddr_in si;
	int len = sizeof(si);
	int sock = UDT::accept(m_socket, (SOCKADDR*)&si, &len);
	ip = inet_ntoa(si.sin_addr);
	port = si.sin_port;
	return sock;
}

bool UDTSocket::Connect(const std::string& host, int port)
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

	return UDT::connect(m_socket, (sockaddr*)&si, sizeof(si)) == 0;
}

bool UDTSocket::Send(char* data, int len)
{
	if(!m_socket)
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
	if(!m_socket)
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
	if(!m_socket)
	{
		return false;
	}

	if(m_udpSocket)
	{
		shutdown(m_udpSocket, SD_BOTH);
		closesocket(m_udpSocket);
	}

	return UDT::close(m_socket) == 0;
}

void UDTSocket::GetIPPort(std::string& ip, int& port)
{
	sockaddr_in addr = {0};
	int len = sizeof(addr);
	if(m_udpSocket)
	{
		if ((m_udpSocket && !getsockname(m_udpSocket, (sockaddr*)&addr, &len)) ||
			!UDT::getsockname(m_socket, (sockaddr*)&addr, &len))
		{
			ip = inet_ntoa(addr.sin_addr);
			port = ntohs(addr.sin_port);
		}
	}
}

int UDTSocket::GetSocket()
{
	return m_socket;
}