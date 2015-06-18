#include "ppp_socket.h"
#include "ppp/ppp_library.h"

PPPSocket::PPPSocket(int socket)
: m_socket(socket)
{
}

PPPSocket::~PPPSocket()
{
	Close();
}
	
bool PPPSocket::Bind(const std::string& host, int port)
{
	if(!m_socket)
	{
		return false;
	}
	
	return true;
}

bool PPPSocket::Listen(int limit)
{
	return false;
}

int PPPSocket::Accept(std::string& ip, int& port)
{
	return 0;
}

bool PPPSocket::Connect(const std::string& host, int port)
{
	if(!m_socket)
	{
		return false;
	}

	return PPPLibrary::GetInstance().Connect(m_socket);
}

bool PPPSocket::Send(char* data, int len)
{
	if(!m_socket)
	{
		return false;
	}

	CSLocker locker(&m_cs);
	int sendlen = len;
	while (sendlen > 0)
	{
		int res = PPPLibrary::GetInstance().Send(m_socket, data + len - sendlen, sendlen);
		if(res <= 0)
		{
			return false;
		}

		sendlen -= res;
	}

	return true;
}

bool PPPSocket::Recv(char* data, int len)
{
	if(!m_socket)
	{
		return false;
	}

	int recvlen = len;
	while (recvlen > 0)
	{
		int res =  PPPLibrary::GetInstance().Recv(m_socket, data + len - recvlen, recvlen);
		if(res <= 0)
		{
			return false;
		}

		recvlen -= res;
	}

	return true;
}

void PPPSocket::GetIPPort(std::string& ip, int& port)
{
	ip = "";
	port = 0;
}

bool PPPSocket::Close()
{
	if(!m_socket)
	{
		return false;
	}

	return PPPLibrary::GetInstance().Disconnect(m_socket);
}

int PPPSocket::GetSocket()
{
	return m_socket;
}
