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
bool PeerType::operator == (const PeerType& peerType) const
{
	return	one_session_id() == peerType.one_session_id() && two_session_id() == peerType.two_session_id() ||
			one_session_id() == peerType.two_session_id() && two_session_id() == peerType.one_session_id();
}
bool PeerType::operator != (const PeerType& peerType) const
{
	return !(operator == (peerType));
}
bool PeerType::operator < (const PeerType& peerType) const
{
	for(size_t i = 0; ; i++)
	{
		if(one_session_id().size() <= i || two_session_id().size() <= i ||
		  peerType.one_session_id().size() <= i || peerType.two_session_id().size() <= i)
			break;
			
		std::string one(one_session_id().c_str(), i + 1);
		std::string two(two_session_id().c_str(), i + 1);
		std::string otherOne(peerType.one_session_id().c_str(), i + 1);
		std::string otherTwo(peerType.two_session_id().c_str(), i + 1);
		
		int resVal = 0, otherResVal = 0;
		for(int j = 0 ; j < (int)one.size(); j++)
		{
			resVal += one[j] + two[j];
			otherResVal += otherOne[j] + otherTwo[j];
		}
		
		int countLess = 0, countMore = 0;
		(one_session_id() < peerType.one_session_id()) ? countLess++ : countMore++;
		(one_session_id() < peerType.two_session_id()) ? countLess++ : countMore++;
		(two_session_id() < peerType.one_session_id()) ? countLess++ : countMore++;
		(two_session_id() < peerType.two_session_id()) ? countLess++ : countMore++;
		
		if(resVal == otherResVal && countLess == countMore)
			continue;
		else
			return ((resVal != otherResVal) ? (resVal < otherResVal) : (countMore < countLess));
	}
	
	return false;
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