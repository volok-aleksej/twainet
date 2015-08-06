#ifndef SOCKET_FACTORIES_H
#define SOCKET_FACTORIES_H

#include "socket_factory.h"
#include <string>

class TCPSocketFactory : public SocketFactory
{
public:
	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);	
	SocketFactory* Clone();
};

class TCPProxySocketFactory : public SocketFactory
{
public:
	TCPProxySocketFactory(const std::string& ip, int port, const std::string& user, const std::string& pass);

	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);
	SocketFactory* Clone();

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
	SocketFactory* Clone();
};

class TCPSecureProxySocketFactory : public SocketFactory
{
public:
	TCPSecureProxySocketFactory(const std::string& ip, int port, const std::string& user, const std::string& pass);

	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);
	SocketFactory* Clone();

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
	SocketFactory* Clone();
};

class UDTSocketFactory : public SocketFactory
{
public:
	UDTSocketFactory();

	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);
	SocketFactory* Clone();

	void SetUdpSocket(int udpSocket);
private:
	int m_udpSocket;
};

class UDTSecureSocketFactory : public SocketFactory
{
public:
	UDTSecureSocketFactory();

	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);
	SocketFactory* Clone();

	void SetUdpSocket(int udpSocket);
private:
	int m_udpSocket;
};

#endif/*SOCKET_FACTORIES_H*/