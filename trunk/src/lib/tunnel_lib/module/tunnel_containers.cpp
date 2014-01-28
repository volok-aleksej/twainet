#include "tunnel_containers.h"
#include "thread/external_connect_thread.h"
#include "connector_lib/thread/listen_thread.h"
#include "connector_lib/connector/connector.h"
#include "thread_lib/thread/thread_manager.h"
#include <time.h>

TunnelConnect::TunnelConnect(const std::string& sessionId)
: m_sessionId(sessionId), m_localListenThread(0)
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
