#ifndef SOCKET_FACTORIES_H
#define SOCKET_FACTORIES_H

#include "socket_factory.h"

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
#endif/*SOCKET_FACTORIES_H*/