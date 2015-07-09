#include "socket_factories.h"
#include "secure_socket.h"
#include "udp_socket.h"
#ifdef WIN32
#	include "ppp_socket.h"
#	include "ppp/ppp_library.h"
#endif/*WIN32*/

/*******************************************************************************************************/
/*                                          TCPSocketFactory                                           */
/*******************************************************************************************************/
AnySocket* TCPSocketFactory::CreateSocket()
{
	return new TCPSocket();
}

AnySocket* TCPSocketFactory::CreateSocket(int socket)
{
	return new TCPSocket(socket);
}

/*******************************************************************************************************/
/*                                      TCPSecureSocketFactory                                         */
/*******************************************************************************************************/
AnySocket* TCPSecureSocketFactory::CreateSocket()
{
	return new SecureTCPSocket();
//	return new TCPSocket();
}

AnySocket* TCPSecureSocketFactory::CreateSocket(int socket)
{
	return new SecureTCPSocket(socket);
//	return new TCPSocket(socket);
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
		return new UDTSocket();
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

/*******************************************************************************************************/
/*                                      UDTSecureSocketFactory                                         */
/*******************************************************************************************************/
AnySocket* UDTSecureSocketFactory::CreateSocket()
{
	if(!m_udpSocket)
	{
		return new SecureUDTSocket();
	}
	else
	{
		return new SecureUDTSocket(m_udpSocket, true);
	}
}

AnySocket* UDTSecureSocketFactory::CreateSocket(int socket)
{
	if(!m_udpSocket)
	{
		return new SecureUDTSocket(socket, false);
	}
	else
	{
		return new SecureUDTSocket(m_udpSocket, socket);
	}
}

void UDTSecureSocketFactory::SetUdpSocket(int udpSocket)
{
	m_udpSocket = udpSocket;
}

#ifdef WIN32
/*******************************************************************************************************/
/*                                          PPPSocketFactory                                           */
/*******************************************************************************************************/
PPPSocketFactory::PPPSocketFactory(const std::string& sessionId)
	: m_sessionId(sessionId){}

AnySocket* PPPSocketFactory::CreateSocket()
{
	int socket = PPPLibrary::GetInstance().CreateSocket(m_sessionId);
	return new PPPSocket(socket);
}

AnySocket* PPPSocketFactory::CreateSocket(int socket)
{
	return new PPPSocket(socket);
}

/*******************************************************************************************************/
/*                                     PPPSecureSocketFactory                                          */
/*******************************************************************************************************/
PPPSecureSocketFactory::PPPSecureSocketFactory(const std::string& sessionId)
	: m_sessionId(sessionId){}

AnySocket* PPPSecureSocketFactory::CreateSocket()
{
	int socket = PPPLibrary::GetInstance().CreateSocket(m_sessionId);
	return new SecurePPPSocket(socket);
}

AnySocket* PPPSecureSocketFactory::CreateSocket(int socket)
{
	return new SecurePPPSocket(socket);
}
#endif/*WIN32*/