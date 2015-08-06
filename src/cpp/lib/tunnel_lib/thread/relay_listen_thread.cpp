#include "relay_listen_thread.h"
#include "common/common_func.h"
#include "connector/tunnel_connector.h"
#include "connector/tunnel_server_connector.h"
#include "message/tunnel_messages.h"
#include "ipc_lib/module/ipc_module.h"
#include "ipc_lib/connector/ipc_connector_factory.h"
#include "thread_lib/thread/thread_manager.h"
#include "connector_lib/socket/socket_factories.h"

//extern std::vector<std::string> GetLocalIps();

RelayListenThread::RelayListenThread(const TunnelServerListenAddress& address)
: m_address(address), m_listenThreadOne(0), m_listenThreadTwo(0)
, m_connectorOne(0), m_connectorTwo(0)
{
}

RelayListenThread::~RelayListenThread()
{
	removeReceiver();

	CSLocker lock(&m_cs);
	if(m_listenThreadOne)
	{
		m_listenThreadOne->Stop();
		ThreadManager::GetInstance().AddThread(m_listenThreadOne);
		m_listenThreadOne = 0;
	}

	if(m_listenThreadTwo)
	{
		m_listenThreadTwo->Stop();
		ThreadManager::GetInstance().AddThread(m_listenThreadTwo);
		m_listenThreadTwo = 0;
	}

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

	delete m_address.m_socketFactory;
}

void RelayListenThread::Stop()
{
	StopThread();
	
	CSLocker lock(&m_cs);
	if(m_listenThreadOne)
	{
		m_listenThreadOne->Stop();
		ThreadManager::GetInstance().AddThread(m_listenThreadOne);
		m_listenThreadOne = 0;
	}

	if(m_listenThreadTwo)
	{
		m_listenThreadTwo->Stop();
		ThreadManager::GetInstance().AddThread(m_listenThreadTwo);
		m_listenThreadTwo = 0;
	}

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

void RelayListenThread::ThreadFunc()
{
	ListenAddress address;
	address.m_id = m_address.m_sessionIdOne;
	address.m_localIP = m_address.m_localIP;
	address.m_localPort = m_address.m_localPort;
	address.m_connectorFactory = new IPCConnectorFactory<TunnelConnector>(m_address.m_sessionIdTwo);
	address.m_socketFactory = m_address.m_socketFactory->Clone();
	address.m_acceptCount = 1;
	m_listenThreadOne = new IPCListenThread(address);
	m_listenThreadOne->addSubscriber(this, SIGNAL_FUNC(this, RelayListenThread, ListenErrorMessage, onListenErrorMessage));
	m_listenThreadOne->addSubscriber(this, SIGNAL_FUNC(this, RelayListenThread, CreatedListenerMessage, onCreatedListenerMessage));
	m_listenThreadOne->addSubscriber(this, SIGNAL_FUNC(this, RelayListenThread, ConnectorMessage, onAddConnector));
	m_listenThreadOne->Start();

	address.m_id = m_address.m_sessionIdTwo;
	address.m_connectorFactory = new IPCConnectorFactory<TunnelConnector>(m_address.m_sessionIdOne);
	address.m_socketFactory = m_address.m_socketFactory->Clone();
	m_listenThreadTwo = new IPCListenThread(address);
	m_listenThreadTwo->addSubscriber(this, SIGNAL_FUNC(this, RelayListenThread, ListenErrorMessage, onListenErrorMessage));
	m_listenThreadTwo->addSubscriber(this, SIGNAL_FUNC(this, RelayListenThread, CreatedListenerMessage, onCreatedListenerMessage));
	m_listenThreadTwo->addSubscriber(this, SIGNAL_FUNC(this, RelayListenThread, ConnectorMessage, onAddConnector));
	m_listenThreadTwo->Start();

	while(!IsStop())
	{
		if(CheckGetAddresses())
		{
			std::string ip;
//			ip = GetLocalIps()[0];//TODO: use external ip
			for(std::map<std::string, Address>::iterator it = m_addresses.begin();
				it != m_addresses.end(); it++)
			{
				CreatedServerListenerMessage msg(m_address.m_id, it->first, ip, it->second.m_localPort);
				onSignal(msg);
			}
			break;
		}
		sleep(100);
	}

	while(!IsStop())
	{
		if(CheckConnect())
		{
			CSLocker lock(&m_cs);
			Connector* conn = new TunnelServerConnector((TunnelConnector*)m_connectorOne, (TunnelConnector*)m_connectorTwo);
			conn->SetId(m_address.m_id);
			m_connectorOne = 0;
			m_connectorTwo = 0;
			ConnectorMessage msg(conn);
			onSignal(msg);
			break;
		}
		sleep(100);
	}
}

void RelayListenThread::onListenErrorMessage(const ListenErrorMessage& msg)
{
	{
		CSLocker lock(&m_cs);
		if(m_listenThreadOne)
		{
			m_listenThreadOne->Stop();
			ThreadManager::GetInstance().AddThread(m_listenThreadOne);
			m_listenThreadOne = 0;
		}

		if(m_listenThreadTwo)
		{
			m_listenThreadTwo->Stop();
			ThreadManager::GetInstance().AddThread(m_listenThreadTwo);
			m_listenThreadTwo = 0;
		}
	}

	SignalError();
}

void RelayListenThread::onCreatedListenerMessage(const CreatedListenerMessage& msg)
{
	Address addr;
	addr.m_localIP = msg.m_ip;
	addr.m_localPort = msg.m_port;
	m_addresses[msg.m_id] = addr;
}

void RelayListenThread::onAddConnector(const ConnectorMessage& msg)
{
	CSLocker lock(&m_cs);
	if(msg.m_conn->GetId() == m_address.m_sessionIdOne)
	{
		m_connectorOne = msg.m_conn;
	}
	else if(msg.m_conn->GetId() == m_address.m_sessionIdTwo)
	{
		m_connectorTwo = msg.m_conn;
	}
}

bool RelayListenThread::CheckGetAddresses()
{
	return m_addresses.size() == 2;
}

bool RelayListenThread::CheckConnect()
{
	CSLocker lock(&m_cs);
	return m_connectorOne && m_connectorTwo;
}

void RelayListenThread::SignalError()
{
	ListenErrorMessage errMsg(m_address.m_id, "", GetError());
	onSignal(errMsg);
}
