#ifndef TUNNEL_CONTAINERS_H
#define TUNNEL_CONTAINERS_H

#include <string>

class ExternalConnectThread;
class ListenThread;
class Thread;

class TunnelConnect
{
public:
	TunnelConnect(const std::string& sessionId);
	~TunnelConnect();

	ListenThread* m_localListenThread;
	ExternalConnectThread* m_externalConnectThread;
	std::string m_sessionId;
	time_t m_creationTime;
};

class TunnelServer
{
public:
	TunnelServer(const std::string& sessionIdOne, const std::string& sessionIdTwo);
	~TunnelServer();

	Thread* m_thread;
	std::string m_sessionIdOne;
	std::string m_sessionIdTwo;
	time_t m_creationTime;
};

#endif/*TUNNEL_CONTAINERS_H*/