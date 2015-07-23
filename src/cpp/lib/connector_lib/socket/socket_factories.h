#ifndef SOCKET_FACTORIES_H
#define SOCKET_FACTORIES_H

#include "socket_factory.h"
#include <string>

class TCPSocketFactory : public SocketFactory
{
public:
	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);
};

class TCPProxySocketFactory : public SocketFactory
{
public:
	TCPProxySocketFactory(const std::string& ip, int port, const std::string& user, const std::string& pass);

	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);

private:
	const std::string& m_ip;
	int m_port;
	const std::string& m_user;
	const std::string& m_pass;
};

class TCPSecureSocketFactory : public SocketFactory
{
public:
	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);
};

class TCPSecureProxySocketFactory : public SocketFactory
{
public:
	TCPSecureProxySocketFactory(const std::string& ip, int port, const std::string& user, const std::string& pass);

	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);

private:
	const std::string& m_ip;
	int m_port;
	const std::string& m_user;
	const std::string& m_pass;
};

class UDPSocketFactory : public SocketFactory
{
public:
	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);
};

class UDTSocketFactory : public SocketFactory
{
public:
	UDTSocketFactory()
		: m_udpSocket(INVALID_SOCKET) {}

	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);

	void SetUdpSocket(int udpSocket);
private:
	int m_udpSocket;
};

class UDTSecureSocketFactory : public SocketFactory
{
public:
	UDTSecureSocketFactory()
		: m_udpSocket(INVALID_SOCKET) {}

	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);

	void SetUdpSocket(int udpSocket);
private:
	int m_udpSocket;
};

#ifdef WIN32
class PPPSocketFactory : public SocketFactory
{
public:
	PPPSocketFactory(const std::string& m_sessionId);

	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);
private:
	std::string m_sessionId;
};

class PPPSecureSocketFactory : public SocketFactory
{
public:
	PPPSecureSocketFactory(const std::string& m_sessionId);

	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);
private:
	std::string m_sessionId;
};
#endif/*WIN32*/

#endif/*SOCKET_FACTORIES_H*/