#include "tunnel_server_connector.h"
#include "thread_lib/thread/thread_manager.h"

TunnelServerConnector::TunnelServerConnector(TunnelConnector* connectorOne, TunnelConnector* connectorTwo)
: m_connectorOne(connectorOne), m_connectorTwo(connectorTwo)
, IPCConnector(0, IPCObjectName(connectorOne->GetId() + "_" + connectorTwo->GetId()))
{
}

TunnelServerConnector::~TunnelServerConnector()
{
}

void TunnelServerConnector::Stop()
{
	StopThread();

	CSLocker lock(&m_cs);
	if(m_connectorOne)
	{
		m_connectorOne->Stop();
		ThreadManager::GetInstance().AddThread(m_connectorOne);
		m_connectorOne = 0;
	}
	if(m_connectorTwo)
	{
		m_connectorTwo->Stop();
		ThreadManager::GetInstance().AddThread(m_connectorTwo);
		m_connectorTwo = 0;
	}
}

void TunnelServerConnector::OnStart()
{
	m_connectorOne->addSubscriber(this, SIGNAL_FUNC(this, TunnelServerConnector, IPCMessageSignal, onIPCMessageOne));
	m_connectorTwo->addSubscriber(this, SIGNAL_FUNC(this, TunnelServerConnector, IPCMessageSignal, onIPCMessageTwo));
	m_connectorOne->addSubscriber(this, SIGNAL_FUNC(this, TunnelServerConnector, ModuleStateMessage, onModuleState));
	m_connectorTwo->addSubscriber(this, SIGNAL_FUNC(this, TunnelServerConnector, ModuleStateMessage, onModuleState));
	m_connectorOne->addSubscriber(this, SIGNAL_FUNC(this, TunnelServerConnector, ModuleNameMessage, onModuleName));
	m_connectorTwo->addSubscriber(this, SIGNAL_FUNC(this, TunnelServerConnector, ModuleNameMessage, onModuleName));
	m_connectorOne->SetServerFlag();
	m_connectorTwo->SetServerFlag();
	m_connectorOne->Start();
	m_connectorTwo->Start();
}

void TunnelServerConnector::OnStop()
{
	CSLocker lock(&m_cs);
	if(m_connectorOne)
	{
		m_connectorOne->Stop();
		ThreadManager::GetInstance().AddThread(m_connectorOne);
		m_connectorOne = 0;
	}
	if(m_connectorTwo)
	{
		m_connectorTwo->Stop();
		ThreadManager::GetInstance().AddThread(m_connectorTwo);
		m_connectorTwo = 0;
	}
}

void TunnelServerConnector::ThreadFunc()
{
	while(!IsStop())
	{
		if (m_connectorOne->IsStopped() ||
			m_connectorTwo->IsStopped())
		{
			sleep(100);
			break;
		}
	}
}

void TunnelServerConnector::onIPCMessageOne(const IPCMessageSignal& msg)
{
	IPCProtoMessage ipcMsg(this, msg);
	m_connectorTwo->toMessage(ipcMsg);
}

void TunnelServerConnector::onIPCMessageTwo(const IPCMessageSignal& msg)
{
	IPCProtoMessage ipcMsg(this, msg);
	m_connectorOne->toMessage(ipcMsg);
}

void TunnelServerConnector::onModuleState(const ModuleStateMessage& msg)
{
	if(msg.id() == m_connectorOne->GetId())
	{
		m_connectorTwo->toMessage(msg);
	}
	else
	{
		m_connectorOne->toMessage(msg);
	}
}

void TunnelServerConnector::onModuleName(const ModuleNameMessage& msg)
{
	if(IPCObjectName(msg.ipc_name()) == IPCObjectName::GetIPCName(m_connectorOne->GetId()))
	{
		m_connectorTwo->toMessage(msg);
	}
	else
	{
		m_connectorOne->toMessage(msg);
	}
}
