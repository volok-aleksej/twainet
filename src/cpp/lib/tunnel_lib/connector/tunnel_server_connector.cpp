#include "tunnel_server_connector.h"
#include "thread_lib/thread/thread_manager.h"

TunnelServerConnector::TunnelServerConnector(TunnelConnector* connectorOne, TunnelConnector* connectorTwo)
: m_connectorOne(connectorOne), m_connectorTwo(connectorTwo)
, IPCConnector(0, IPCObjectName(connectorOne->GetId() + "_" + connectorTwo->GetId()))
{
}

TunnelServerConnector::~TunnelServerConnector()
{
	ThreadManager::GetInstance().AddThread(m_connectorOne);
	ThreadManager::GetInstance().AddThread(m_connectorTwo);
	removeReceiver();
}

void TunnelServerConnector::Stop()
{
	StopThread();
	OnStop();
}

void TunnelServerConnector::OnStart()
{
	m_connectorOne->addSubscriber(this, SIGNAL_FUNC(this, TunnelServerConnector, IPCMessageSignal, onIPCMessageOne));
	m_connectorTwo->addSubscriber(this, SIGNAL_FUNC(this, TunnelServerConnector, IPCMessageSignal, onIPCMessageTwo));
	m_connectorOne->addSubscriber(this, SIGNAL_FUNC(this, TunnelServerConnector, ModuleStateMessage, onModuleState));
	m_connectorTwo->addSubscriber(this, SIGNAL_FUNC(this, TunnelServerConnector, ModuleStateMessage, onModuleState));
	m_connectorOne->addSubscriber(this, SIGNAL_FUNC(this, TunnelServerConnector, ModuleNameMessage, onModuleName));
	m_connectorTwo->addSubscriber(this, SIGNAL_FUNC(this, TunnelServerConnector, ModuleNameMessage, onModuleName));
	m_connectorOne->addSubscriber(this, SIGNAL_FUNC(this, TunnelServerConnector, InternalConnectionDataMessage, onInternalConnectionData));
	m_connectorTwo->addSubscriber(this, SIGNAL_FUNC(this, TunnelServerConnector, InternalConnectionDataMessage, onInternalConnectionData));
	m_connectorOne->addSubscriber(this, SIGNAL_FUNC(this, TunnelServerConnector, InternalConnectionStatusMessage, onInternalConnectionStatus));
	m_connectorTwo->addSubscriber(this, SIGNAL_FUNC(this, TunnelServerConnector, InternalConnectionStatusMessage, onInternalConnectionStatus));
	m_connectorOne->addSubscriber(this, SIGNAL_FUNC(this, TunnelServerConnector, InitInternalConnectionMessage, onInitInternalConnection));
	m_connectorTwo->addSubscriber(this, SIGNAL_FUNC(this, TunnelServerConnector, InitInternalConnectionMessage, onInitInternalConnection));
	m_connectorOne->SetServerFlag();
	m_connectorTwo->SetServerFlag();
	m_connectorOne->Start();
	m_connectorTwo->Start();
}

void TunnelServerConnector::OnStop()
{
	m_connectorOne->Stop();
	m_connectorTwo->Stop();
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

bool TunnelServerConnector::SetModuleName(const IPCObjectName& moduleName)
{
	return false;
}

void TunnelServerConnector::onIPCMessageOne(const IPCMessageSignal& msg)
{
	IPCProtoMessage ipcMsg(&m_handler, msg);
	m_connectorTwo->toMessage(ipcMsg);
}

void TunnelServerConnector::onIPCMessageTwo(const IPCMessageSignal& msg)
{
	IPCProtoMessage ipcMsg(&m_handler, msg);
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
	if(msg.ipc_name().host_name() == m_connectorOne->GetId())
	{
		m_connectorTwo->toMessage(msg);
	}
	else
	{
		m_connectorOne->toMessage(msg);
	}
}

void TunnelServerConnector::onInternalConnectionData(const InternalConnectionDataMessage& msg)
{
	IPCObjectName target(msg.target());
	const_cast<InternalConnectionDataMessage&>(msg).release_target();
	if(target.GetModuleNameString() == m_connectorOne->GetId())
	{
		m_connectorTwo->toMessage(msg);
	}
	else
	{
		m_connectorOne->toMessage(msg);
	}
}

void TunnelServerConnector::onInternalConnectionStatus(const InternalConnectionStatusMessage& msg)
{
	IPCObjectName target(msg.target());
	const_cast<InternalConnectionStatusMessage&>(msg).release_target();
	if(target.GetModuleNameString() == m_connectorOne->GetId())
	{
		m_connectorTwo->toMessage(msg);
	}
	else
	{
		m_connectorOne->toMessage(msg);
	}
}

void TunnelServerConnector::onInitInternalConnection(const InitInternalConnectionMessage& msg)
{
	IPCObjectName target(msg.target());
	const_cast<InitInternalConnectionMessage&>(msg).release_target();
	if(target.GetModuleNameString() == m_connectorOne->GetId())
	{
		m_connectorTwo->toMessage(msg);
	}
	else
	{
		m_connectorOne->toMessage(msg);
	}
}