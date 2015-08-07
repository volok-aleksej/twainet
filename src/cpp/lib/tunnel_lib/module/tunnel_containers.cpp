#include "tunnel_containers.h"
#include "thread/external_connect_thread.h"
#include "connector_lib/thread/listen_thread.h"
#include "connector_lib/connector/connector.h"
#include "connector_lib/socket/udp_socket.h"
#include "thread_lib/thread/thread_manager.h"
#include <time.h>

TunnelConnect::TunnelConnect(const std::string& sessionId)
: m_sessionId(sessionId), m_localListenThread(0), m_localUdpSocket(0)
, m_externalConnectThread(0), m_creationTime(0)
{
	time(&m_creationTime);
}

TunnelConnect::~TunnelConnect()
{
	std::vector<Thread*> threads;
	threads.push_back(m_localListenThread);
	threads.push_back(m_externalConnectThread);
	for(std::vector<Thread*>::iterator it = threads.begin();
		it != threads.end(); it++)
	{
		if(*it)
		{
			(*it)->Stop();
			ThreadManager::GetInstance().AddThread(*it);
		}
	}

	if(m_localUdpSocket)
	{
		m_localUdpSocket->Close();
	}

	m_localListenThread = 0;
	m_externalConnectThread = 0;
}

TunnelServer::TunnelServer(const std::string& sessionIdOne, const std::string& sessionIdTwo)
: m_sessionIdOne(sessionIdOne), m_sessionIdTwo(sessionIdTwo), m_thread(0), m_creationTime(0)
{
	time(&m_creationTime);
}
TunnelServer::~TunnelServer()
{
	if(m_thread)
	{
		m_thread->Stop();
		ThreadManager::GetInstance().AddThread(m_thread);
		m_thread = 0;
	}
}

PeerType::PeerType(){}
PeerType::PeerType(const PeerData& data)
{
	PeerData::operator = (data);
}
PeerType::~PeerType(){}
bool PeerType::operator == (const PeerType& peerType)
{
	return	one_session_id() == peerType.one_session_id() && two_session_id() == peerType.two_session_id() ||
			one_session_id() == peerType.two_session_id() && two_session_id() == peerType.one_session_id();
}
bool PeerType::operator != (const PeerType& peerType)
{
	return !(operator == (peerType));
}
bool PeerType::operator < (const PeerType& peerType) const
{
	int resVal = 0, otherResVal = 0;
	for(std::string::const_iterator it = one_session_id().begin();
		it != one_session_id().end(); it++)
	{
		resVal += *it;
	}
	for(std::string::const_iterator it = peerType.one_session_id().begin();
		it != peerType.one_session_id().end(); it++)
	{
		otherResVal += *it;
	}

	return resVal < otherResVal;
}

TunnelStep::TunnelStep()
{
	time(&m_creationTime);
}
TunnelStep::TunnelStep(const TunnelStep& data)
{
	operator = (data);
}
TunnelStep::TunnelStep(const PeerData& data)
{
	PeerData::operator = (data);
	time(&m_creationTime);
}
TunnelStep::~TunnelStep(){}
void TunnelStep::operator = (const TunnelStep& step)
{
	PeerData::operator=(step);
	m_creationTime = step.m_creationTime;
}