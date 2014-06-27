#include "socket_factories.h"
#include "tcp_socket.h"
#include "udp_socket.h"
#include "udt_socket.h"

AnySocket* TCPSocketFactory::CreateSocket()
{
	return new TCPSocket(AnySocket::COMMON_SOCKET);
}

AnySocket* TCPSocketFactory::CreateSocket(int socket)
{
	return new TCPSocket(AnySocket::COMMON_SOCKET, socket);
}

AnySocket* TCPSecureSocketFactory::CreateSocket()
{
	return new TCPSocket(AnySocket::SECURE_SOCKET);
}

AnySocket* TCPSecureSocketFactory::CreateSocket(int socket)
{
	return new TCPSocket(AnySocket::SECURE_SOCKET, socket);
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
		return new UDTSocket(AnySocket::COMMON_SOCKET);
	}
	else
	{
		return new UDTSocket(m_udpSocket, true, AnySocket::COMMON_SOCKET);
	}
}

AnySocket* UDTSocketFactory::CreateSocket(int socket)
{
	if(!m_udpSocket)
	{
		return new UDTSocket(socket, false, AnySocket::COMMON_SOCKET);
	}
	else
	{
		return new UDTSocket(m_udpSocket, socket, AnySocket::COMMON_SOCKET);
	}
}

void UDTSocketFactory::SetUdpSocket(int udpSocket)
{
	m_udpSocket = udpSocket;
}
