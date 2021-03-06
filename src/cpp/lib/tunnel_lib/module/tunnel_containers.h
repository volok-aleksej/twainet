#ifndef TUNNEL_CONTAINERS_H
#define TUNNEL_CONTAINERS_H

#include <string>

#pragma warning(disable:4244 4267)
#include "../messages/client_server.pb.h"
using namespace client_server;
#pragma warning(default:4244 4267)

class ExternalConnectThread;
class ListenThread;
class Thread;
class UDPSocket;

class TunnelConnect
{
public:
	TunnelConnect(const std::string& sessionId);
	~TunnelConnect();

	ListenThread* m_localListenThread;
	UDPSocket* m_localUdpSocket;
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

class PeerType : public PeerData
{
public:
	PeerType();
	PeerType(const PeerData& data);
	virtual ~PeerType();

	bool operator == (const PeerType& peerType) const;
	bool operator != (const PeerType& peerType) const;
	bool operator < (const PeerType& peerType) const;
};

class TunnelStep : public PeerType
{
public:
	TunnelStep();
	TunnelStep(const TunnelStep& data);
	TunnelStep(const PeerData& data);
	virtual ~TunnelStep();
	
	void operator = (const TunnelStep& step);
	
	time_t m_creationTime;
};

class AvailableTypes : public AvailablePearTypes
{
public:
    AvailableTypes();
    AvailableTypes(const AvailablePearTypes& types);
    virtual ~AvailableTypes();
    
    bool operator == (const AvailableTypes& types) const;
    bool operator != (const AvailableTypes& types) const;
    bool operator < (const AvailableTypes& types) const;
};

#endif/*TUNNEL_CONTAINERS_H*/