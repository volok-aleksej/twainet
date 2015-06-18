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

class TCPSecureSocketFactory : public SocketFactory
{
public:
	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);
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
	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);

	void SetUdpSocket(int udpSocket);
private:
	int m_udpSocket;
};

class UDTSecureSocketFactory : public SocketFactory
{
public:
	AnySocket* CreateSocket();
	AnySocket* CreateSocket(int socket);

	void SetUdpSocket(int udpSocket);
private:
	int m_udpSocket;
};

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

#endif/*SOCKET_FACTORIES_H*/