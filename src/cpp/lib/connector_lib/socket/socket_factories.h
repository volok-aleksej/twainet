#ifndef SOCKET_FACTORIES_H
#define SOCKET_FACTORIES_H

#include "socket_factory.h"
#include <string>

class TCPSocketFactory : public SocketFactory
{
public:
	TCPSocketFactory(int ipv);
	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);	
	SocketFactory* Clone();
private:
	int m_ipv;
};

class TCPProxySocketFactory : public SocketFactory
{
public:
	TCPProxySocketFactory(const std::string& ip, int port, const std::string& user, const std::string& pass, int ipv);

	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);
	SocketFactory* Clone();

private:
	const std::string& m_ip;
	int m_port;
	const std::string& m_user;
	const std::string& m_pass;
	int m_ipv;
};

class TCPSecureSocketFactory : public SocketFactory
{
public:
	TCPSecureSocketFactory(int ipv);
	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);
	SocketFactory* Clone();
private:
	int m_ipv;
};

class TCPSecureProxySocketFactory : public SocketFactory
{
public:
	TCPSecureProxySocketFactory(const std::string& ip, int port, const std::string& user, const std::string& pass, int ipv);

	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);
	SocketFactory* Clone();

private:
	const std::string& m_ip;
	int m_port;
	const std::string& m_user;
	const std::string& m_pass;
	int m_ipv;
};

class UDPSocketFactory : public SocketFactory
{
public:
	UDPSocketFactory(int ipv);
	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);
	SocketFactory* Clone();
private:
	int m_ipv;
};

class UDTSocketFactory : public SocketFactory
{
public:
	UDTSocketFactory(int ipv);

	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);
	SocketFactory* Clone();

	void SetUdpSocket(int udpSocket);
private:
	int m_udpSocket;
	int m_ipv;
};

class UDTSecureSocketFactory : public SocketFactory
{
public:
	UDTSecureSocketFactory(int ipv);

	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);
	SocketFactory* Clone();

	void SetUdpSocket(int udpSocket);
private:
	int m_udpSocket;
	int m_ipv;
};

#endif/*SOCKET_FACTORIES_H*/