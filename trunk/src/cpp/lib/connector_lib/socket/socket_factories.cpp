#include "socket_factories.h"
#include "tcp_socket.h"
#include "udp_socket.h"
#include "udt_socket.h"

/*******************************************************************************************************/
/*                                          TCPSocketFactory                                           */
/*******************************************************************************************************/
AnySocket* TCPSocketFactory::CreateSocket()
{
	return new TCPSocket(AnySocket::COMMON_SOCKET);
}

AnySocket* TCPSocketFactory::CreateSocket(int socket)
{
	return new TCPSocket(AnySocket::COMMON_SOCKET, socket);
}

/*******************************************************************************************************/
/*                                      TCPSecureSocketFactory                                         */
/*******************************************************************************************************/
AnySocket* TCPSecureSocketFactory::CreateSocket()
{
	return new TCPSocket(AnySocket::SECURE_SOCKET);
//	return new TCPSocket(AnySocket::COMMON_SOCKET);
}

AnySocket* TCPSecureSocketFactory::CreateSocket(int socket)
{
	return new TCPSocket(AnySocket::SECURE_SOCKET, socket);
//	return new TCPSocket(AnySocket::COMMON_SOCKET, socket);
}

/*******************************************************************************************************/
/*                                         UDPSocketFactory                                            */
/*******************************************************************************************************/
AnySocket* UDPSocketFactory::CreateSocket()
{
	return new UDPSocket;
}

AnySocket* UDPSocketFactory::CreateSocket(int socket)
{
	return new UDPSocket(socket);
}

/*******************************************************************************************************/
/*                                         UDTSocketFactory                                            */
/*******************************************************************************************************/
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

/*******************************************************************************************************/
/*                                      UDTSecureSocketFactory                                         */
/*******************************************************************************************************/
AnySocket* UDTSecureSocketFactory::CreateSocket()
{
	if(!m_udpSocket)
	{
		return new UDTSocket(AnySocket::SECURE_SOCKET);
	}
	else
	{
		return new UDTSocket(m_udpSocket, true, AnySocket::SECURE_SOCKET);
	}
}

AnySocket* UDTSecureSocketFactory::CreateSocket(int socket)
{
	if(!m_udpSocket)
	{
		return new UDTSocket(socket, false, AnySocket::SECURE_SOCKET);
	}
	else
	{
		return new UDTSocket(m_udpSocket, socket, AnySocket::SECURE_SOCKET);
	}
}

void UDTSecureSocketFactory::SetUdpSocket(int udpSocket)
{
	m_udpSocket = udpSocket;
}
