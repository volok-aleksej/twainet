#include "socket_factories.h"
#include "secure_socket.h"
#include "proxy_socket.h"
#include "udp_socket.h"

#ifndef INVALID_SOCKET
#	define INVALID_SOCKET -1
#endif/*INVALID_SOCKET*/

/*******************************************************************************************************/
/*                                          TCPSocketFactory                                           */
/*******************************************************************************************************/
TCPSocketFactory::TCPSocketFactory(int ipv)
: m_ipv(ipv){}

AnySocket* TCPSocketFactory::CreateSocket()
{
	return new TCPSocket((AnySocket::IPVersion)m_ipv);
}

AnySocket* TCPSocketFactory::CreateSocket(int socket)
{
	return new TCPSocket(socket);
}

SocketFactory* TCPSocketFactory::Clone()
{
	return new TCPSocketFactory(m_ipv);
}

/*******************************************************************************************************/
/*                                      TCPProxySocketFactory                                          */
/*******************************************************************************************************/
TCPProxySocketFactory::TCPProxySocketFactory(const std::string& ip, int port, const std::string& user, const std::string& pass, int ipv)
: m_ip(ip), m_port(port), m_user(user), m_pass(pass), m_ipv(ipv){}

AnySocket* TCPProxySocketFactory::CreateSocket()
{
	ProxyTCPSocket* socket = new ProxyTCPSocket(m_ip, m_port, (AnySocket::IPVersion)m_ipv);
	socket->SetUserName(m_user);
	socket->SetPassword(m_pass);
	return socket;
}

AnySocket* TCPProxySocketFactory::CreateSocket(int socket)
{
	ProxyTCPSocket* new_socket = new ProxyTCPSocket(socket, m_ip, m_port);
	new_socket->SetUserName(m_user);
	new_socket->SetPassword(m_pass);
	return new_socket;
}

SocketFactory* TCPProxySocketFactory::Clone()
{
	return new TCPProxySocketFactory(m_ip, m_port, m_user, m_pass, m_ipv);
}

/*******************************************************************************************************/
/*                                      TCPSecureSocketFactory                                         */
/*******************************************************************************************************/
TCPSecureSocketFactory::TCPSecureSocketFactory(int ipv)
: m_ipv(ipv){}

AnySocket* TCPSecureSocketFactory::CreateSocket()
{
	return new SecureTCPSocket((AnySocket::IPVersion)m_ipv);
}

AnySocket* TCPSecureSocketFactory::CreateSocket(int socket)
{
	return new SecureTCPSocket(socket);
}

SocketFactory* TCPSecureSocketFactory::Clone()
{
	return new TCPSecureSocketFactory(m_ipv);
}

/*******************************************************************************************************/
/*                                  TCPSecureProxySocketFactory                                        */
/*******************************************************************************************************/
TCPSecureProxySocketFactory::TCPSecureProxySocketFactory(const std::string& ip, int port, const std::string& user, const std::string& pass, int ipv)
: m_ip(ip), m_port(port), m_user(user), m_pass(pass), m_ipv(ipv){}

AnySocket* TCPSecureProxySocketFactory::CreateSocket()
{
	SecureProxyTCPSocket* socket = new SecureProxyTCPSocket(m_ip, m_port, (AnySocket::IPVersion)m_ipv);
	socket->SetUserName(m_user);
	socket->SetPassword(m_pass);
	return socket;
}

AnySocket* TCPSecureProxySocketFactory::CreateSocket(int socket)
{
	SecureProxyTCPSocket* new_socket = new SecureProxyTCPSocket(socket, m_ip, m_port);
	new_socket->SetUserName(m_user);
	new_socket->SetPassword(m_pass);
	return new_socket;
}

SocketFactory* TCPSecureProxySocketFactory::Clone()
{
	return new TCPSecureProxySocketFactory(m_ip, m_port, m_user, m_pass, m_ipv);
}

/*******************************************************************************************************/
/*                                         UDPSocketFactory                                            */
/*******************************************************************************************************/
UDPSocketFactory::UDPSocketFactory(int ipv)
: m_ipv(ipv){}

AnySocket* UDPSocketFactory::CreateSocket()
{
	return new UDPSocket((AnySocket::IPVersion)m_ipv);
}

AnySocket* UDPSocketFactory::CreateSocket(int socket)
{
	return new UDPSocket(socket);
}

SocketFactory* UDPSocketFactory::Clone()
{
	return new UDPSocketFactory(m_ipv);
}

/*******************************************************************************************************/
/*                                         UDTSocketFactory                                            */
/*******************************************************************************************************/
UDTSocketFactory::UDTSocketFactory(int ipv)
: m_udpSocket(INVALID_SOCKET), m_ipv(ipv) {}

AnySocket* UDTSocketFactory::CreateSocket()
{
	if(m_udpSocket != INVALID_SOCKET)
	{
		return new UDTSocket((AnySocket::IPVersion)m_ipv);
	}
	else
	{
		return new UDTSocket(m_udpSocket, (AnySocket::IPVersion)m_ipv, true);
	}
}

AnySocket* UDTSocketFactory::CreateSocket(int socket)
{
	if(m_udpSocket != INVALID_SOCKET)
	{
		return new UDTSocket(socket, (AnySocket::IPVersion)m_ipv, false);
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

SocketFactory* UDTSocketFactory::Clone()
{
	UDTSocketFactory* factory = new UDTSocketFactory(m_ipv);
	if(m_udpSocket != INVALID_SOCKET)
		factory->SetUdpSocket(m_udpSocket);
	return factory;
}

/*******************************************************************************************************/
/*                                      UDTSecureSocketFactory                                         */
/*******************************************************************************************************/
UDTSecureSocketFactory::UDTSecureSocketFactory(int ipv)
: m_udpSocket(INVALID_SOCKET), m_ipv(ipv){}

AnySocket* UDTSecureSocketFactory::CreateSocket()
{
	if(m_udpSocket == INVALID_SOCKET)
	{
		return new SecureUDTSocket((AnySocket::IPVersion)m_ipv);
	}
	else
	{
		return new SecureUDTSocket(m_udpSocket, (AnySocket::IPVersion)m_ipv, true);
	}
}

AnySocket* UDTSecureSocketFactory::CreateSocket(int socket)
{
	if(m_udpSocket == INVALID_SOCKET)
	{
		return new SecureUDTSocket(socket, (AnySocket::IPVersion)m_ipv, false);
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

SocketFactory* UDTSecureSocketFactory::Clone()
{
	UDTSecureSocketFactory* factory = new UDTSecureSocketFactory(m_ipv);
	if(m_udpSocket != INVALID_SOCKET)
		factory->SetUdpSocket(m_udpSocket);
	return factory;
}