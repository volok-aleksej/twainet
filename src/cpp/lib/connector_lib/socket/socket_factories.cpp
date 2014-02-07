#include "socket_factories.h"
#include "tcp_socket.h"
#include "udp_socket.h"
#include "udt_socket.h"

AnySocket* TCPSocketFactory::CreateSocket()
{
	return new TCPSocket(TCPSocket::COMMON_SOCKET);
}

AnySocket* TCPSocketFactory::CreateSocket(int socket)
{
	return new TCPSocket(TCPSocket::COMMON_SOCKET, socket);
}

AnySocket* TCPSecureSocketFactory::CreateSocket()
{
	return new TCPSocket(TCPSocket::SECURE_SOCKET);
}

AnySocket* TCPSecureSocketFactory::CreateSocket(int socket)
{
	return new TCPSocket(TCPSocket::SECURE_SOCKET, socket);
}

AnySocket* UDPSocketFactory::CreateSocket()
{
	return new UDPSocket;
}

AnySocket* UDPSocketFactory::CreateSocket(int socket)
{
	return new UDPSocket(socket);
}

AnySocket* UDTSocketFactory::CreateSocket()
{
	if(!m_udpSocket)
	{
		return new UDTSocket;
	}
	else
	{
		return new UDTSocket(m_udpSocket, true);
	}
}

AnySocket* UDTSocketFactory::CreateSocket(int socket)
{
	if(!m_udpSocket)
	{
		return new UDTSocket(socket, false);
	}
	else
	{
		return new UDTSocket(m_udpSocket, socket);
	}
}

void UDTSocketFactory::SetUdpSocket(int udpSocket)
{
	m_udpSocket = udpSocket;
}
