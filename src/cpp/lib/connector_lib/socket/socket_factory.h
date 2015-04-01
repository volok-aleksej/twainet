#ifndef SOCKET_FACTORY_H
#define SOCKET_FACTORY_H

class AnySocket;

class SocketFactory
{
public:
	virtual AnySocket* CreateSocket() = 0;
	virtual AnySocket* CreateSocket(int socket) = 0;
};

#endif/*SOCKET_FACTORY_H*/