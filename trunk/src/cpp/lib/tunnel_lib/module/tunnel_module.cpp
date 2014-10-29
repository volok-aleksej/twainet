#include <time.h>
#include "tunnel_module.h" 
#include "connector/tunnel_connector.h"
#include "../thread/external_listen_thread.h"
#include "../thread/external_connect_thread.h"
#include "../thread/relay_listen_thread.h"
#include "thread_lib/thread/thread_manager.h"
#include "ipc_lib/connector/ipc_connector_factory.h"
#include "connector_lib/socket/socket_factories.h"
#include "common/guid_generator.h"

extern std::vector<std::string> GetLocalIps();

TunnelModule::TunnelModule(const IPCObjectName& ipcName, ConnectorFactory* factory)
: ClientServerModule(ipcName, factory), m_tunnelChecker(0)
{
	m_tunnelChecker = new TunnelCheckerThread(this);
	m_tunnelChecker->Start();
}

TunnelModule::~TunnelModule()
{
	removeReceiver();

	m_tunnelChecker->Join();
	delete m_tunnelChecker;

	CSLocker lock(&m_cs);
	for(std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.begin();
		it != m_tunnels.end(); it++)
	{
		delete it->second;
	}

	for(std::map<std::string, TunnelServer*>::iterator it = m_servers.begin();
		it != m_servers.end(); it++)
	{
		delete it->second;
	}
}

void TunnelModule::InitNewTunnel(const std::string& extSessionId)
{
	printf("init new tunnel - sessionId: %s\n", extSessionId.c_str());
	InitTunnel itMsg;
	itMsg.set_own_session_id(m_ownSessionId);
	itMsg.set_ext_session_id(extSessionId);
	InitTunnelSignal itSig(itMsg);
	onSignal(itSig);
}

void TunnelModule::DestroyTunnel(const std::string& extSessionId)
{
	m_manager.StopConnection(extSessionId);
}

void TunnelModule::OnNewConnector(Connector* connector)
{
	ClientServerConnector* clientServerConn = dynamic_cast<ClientServerConnector*>(const_cast<Connector*>(connector));
	if(clientServerConn)
	{
		ipcSubscribe(clientServerConn, SIGNAL_FUNC(this, TunnelModule, InitTunnelMessage, onInitTunnel)); //for client
		ipcSubscribe(clientServerConn, SIGNAL_FUNC(this, TunnelModule, InitTunnelSignal, onInitTunnel)); //for server
		ipcSubscribe(clientServerConn, SIGNAL_FUNC(this, TunnelModule, TryConnectToMessage, onTryConnectTo)); //for client
		ipcSubscribe(clientServerConn, SIGNAL_FUNC(this, TunnelModule, InitTunnelStartedMessage, onInitTunnelStarted)); //for client
	}

	TunnelConnector* tunnelConnector = dynamic_cast<TunnelConnector*>(connector);
	if(tunnelConnector)
	{
		ipcSubscribe(tunnelConnector, SIGNAL_FUNC(this, TunnelModule, ModuleNameMessage, onModuleName));
		ipcSubscribe(tunnelConnector, SIGNAL_FUNC(this, TunnelModule, IPCProtoMessage, onIPCMessage));
	}
}

void TunnelModule::TunnelConnectFailed(const std::string& sessionId)
{
	printf("Creation of tunnel failed - sessionId: %s\n", sessionId.c_str());
}

void TunnelModule::TunnelConnected(const std::string& sessionId)
{
	printf("Creation of tunnel successful - sessionId: %s\n", sessionId.c_str());
}

void TunnelModule::onInitTunnel(const InitTunnelMessage& msg)
{
	if(!msg.has_type())
	{
		return;
	}

	if(msg.type() == TUNNEL_LOCAL)
	{
		CreateLocalListenThread(msg.ext_session_id());
	}
	else if(msg.type() == TUNNEL_EXTERNAL)
	{
		InitExternalConnectThread(msg.ext_session_id(), msg.address().ip(), msg.address().port());
	}
	else if(msg.type() == TUNNEL_RELAY)
	{
		CreateRelayConnectThread(msg.ext_session_id(), msg.address().ip(), msg.address().port());
	}
}

void TunnelModule::onInitTunnel(const InitTunnelSignal& msg)
{
	InitTunnelStarted itsMsg;
	itsMsg.set_own_session_id(msg.own_session_id());
	itsMsg.set_ext_session_id(msg.ext_session_id());
	InitTunnelStartedSignal itrSig(itsMsg);
	onSignal(itrSig);
	itrSig.set_own_session_id(msg.ext_session_id());
	itrSig.set_ext_session_id(msg.own_session_id());
	onSignal(itrSig);

	InitTunnelSignal itMsg(msg);
	itMsg.set_type(TUNNEL_LOCAL);
	onSignal(itMsg);
	itMsg.set_own_session_id(msg.ext_session_id());
	itMsg.set_ext_session_id(msg.own_session_id());
	onSignal(itMsg);

	SignalOwner* threadOwner = 0;
	TunnelServerListenAddress address;
	address.m_localIP = "";
	address.m_localPort = 0;
	address.m_sessionIdOne = msg.own_session_id();
	address.m_sessionIdTwo = msg.ext_session_id();
	address.m_id = CreateGUID();
	address.m_socketFactory = new UDPSocketFactory;
	TunnelServer* external = new TunnelServer(msg.own_session_id(), msg.ext_session_id());
	external->m_thread = new ExternalListenThread(address);
	m_servers.insert(std::make_pair(address.m_id, external));
	threadOwner = dynamic_cast<SignalOwner*>(external->m_thread);
	threadOwner->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, CreatedServerListenerMessage, onCreatedExternalListener));
	threadOwner->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, GotExternalAddressMessage, onGotExternalAddress));
	threadOwner->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ListenErrorMessage, onErrorExternalListener));
	external->m_thread->Start();
	
	address.m_id = CreateGUID();
	address.m_socketFactory = 0;
	TunnelServer* relay = new TunnelServer(msg.own_session_id(), msg.ext_session_id());
	relay->m_thread = new RelayListenThread(address);
	m_servers.insert(std::make_pair(address.m_id, relay));
	threadOwner = dynamic_cast<SignalOwner*>(relay->m_thread);
	threadOwner->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, CreatedServerListenerMessage, onCreatedRelayListener));
	threadOwner->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ListenErrorMessage, onErrorRelayListener));
	threadOwner->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ConnectorMessage, onAddRelayServerConnector));
	relay->m_thread->Start();
}

void TunnelModule::onTryConnectTo(const TryConnectToMessage& msg)
{
	if(msg.type() == TUNNEL_LOCAL)
	{
		for(int i = 0; i < msg.adresses_size(); i++)
		{
			CreateLocalConnectThread(msg.ext_session_id(), msg.adresses(i).ip(), msg.adresses(i).port());
		}
	}
	else if(msg.type() == TUNNEL_EXTERNAL)
	{
		for(int i = 0; i < msg.adresses_size(); i++)
		{
			CreateExternalConnectThread(msg.ext_session_id(), msg.adresses(i).ip(), msg.adresses(i).port());
		}
	}
}

void TunnelModule::onInitTunnelStarted(const InitTunnelStartedMessage& msg)
{
	CSLocker lock(&m_cs);
	TunnelConnect* tunnel;
	std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.find(msg.ext_session_id());
	if(it == m_tunnels.end())
	{
		tunnel = new TunnelConnect(msg.ext_session_id());
		m_tunnels.insert(std::make_pair(msg.ext_session_id(), tunnel));
	}
}

void TunnelModule::onCreatedLocalListener(const CreatedListenerMessage& msg)
{
	TryConnectTo tctMsg;
	tctMsg.set_type(TUNNEL_LOCAL);
	tctMsg.set_own_session_id(m_ownSessionId);
	tctMsg.set_ext_session_id(msg.m_id);
	std::vector<std::string> ips = GetLocalIps();
	for(size_t i = 0; i < ips.size(); i++)
	{
		TunnelConnectAddress* address = tctMsg.add_adresses();
		address->set_ip(ips[i]);
		address->set_port(msg.m_port);
	}

	TryConnectToSignal tctSig(tctMsg);
	onSignal(tctSig);
}

void TunnelModule::onErrorLocalListener(const ListenErrorMessage& msg)
{
	CSLocker lock(&m_cs);
	std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.find(msg.m_id);
	if(it != m_tunnels.end())
	{
		it->second->m_localListenThread->Stop();
		ThreadManager::GetInstance().AddThread(it->second->m_localListenThread);
		it->second->m_localListenThread = 0;
	}
}

void TunnelModule::onErrorLocalConnect(const ConnectErrorMessage& msg)
{
}

void TunnelModule::onAddLocalConnector(const ConnectorMessage& msg)
{
//	printf("new local connector - tunnel\n");
	IPCModule::onAddConnector(msg);
	//TODO: check another connections
}

void TunnelModule::onErrorExternalConnect(const ConnectErrorMessage& msg)
{
	CSLocker lock(&m_cs);
	std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.find(msg.m_moduleName);
	if(it != m_tunnels.end())
	{
		it->second->m_externalConnectThread->Stop();
		ThreadManager::GetInstance().AddThread(it->second->m_externalConnectThread);
		it->second->m_externalConnectThread = 0;
	}
}

void TunnelModule::onAddExternalConnector(const ConnectorMessage& msg)
{
//	printf("new external connector - tunnel\n");
	IPCModule::onAddConnector(msg);
	//TODO: check another connections
}

void TunnelModule::onCreatedExternalListener(const CreatedServerListenerMessage& msg)
{
	CSLocker lock(&m_cs);
	std::map<std::string, TunnelServer*>::iterator it = m_servers.find(msg.m_id);
	if(it == m_servers.end())
	{
		return;
	}

	InitTunnel itMsg;
	itMsg.set_type(TUNNEL_EXTERNAL);
	if(msg.m_sessionId == it->second->m_sessionIdOne)
	{
		itMsg.set_own_session_id(it->second->m_sessionIdOne);
		itMsg.set_ext_session_id(it->second->m_sessionIdTwo);
	}
	else
	{
		itMsg.set_own_session_id(it->second->m_sessionIdTwo);
		itMsg.set_ext_session_id(it->second->m_sessionIdOne);
	}
	TunnelConnectAddress* address = itMsg.mutable_address();
	address->set_ip(msg.m_ip);
	address->set_port(msg.m_port);
	InitTunnelSignal itSig(itMsg);
	onSignal(itSig);

}

void TunnelModule::onGotExternalAddress(const GotExternalAddressMessage& msg)
{
	CSLocker lock(&m_cs);
	std::map<std::string, TunnelServer*>::iterator it = m_servers.find(msg.m_id);
	if(it == m_servers.end())
	{
		return;
	}
	
	TryConnectTo tctMsg;
	tctMsg.set_type(TUNNEL_EXTERNAL);
	if(msg.m_sessionId != it->second->m_sessionIdOne)
	{
		tctMsg.set_own_session_id(it->second->m_sessionIdOne);
		tctMsg.set_ext_session_id(it->second->m_sessionIdTwo);
	}
	else
	{
		tctMsg.set_own_session_id(it->second->m_sessionIdTwo);
		tctMsg.set_ext_session_id(it->second->m_sessionIdOne);
	}
	TunnelConnectAddress* address = tctMsg.add_adresses();
	address->set_ip(msg.m_ip);
	address->set_port(msg.m_port);
	TryConnectToSignal tctSig(tctMsg);
	onSignal(tctSig);
}

void TunnelModule::onErrorExternalListener(const ListenErrorMessage& msg)
{
	CSLocker lock(&m_cs);
	std::map<std::string, TunnelServer*>::iterator it = m_servers.find(msg.m_id);
	if(it != m_servers.end())
	{
		delete it->second;
		m_servers.erase(it);
	}	
}

void TunnelModule::onCreatedRelayListener(const CreatedServerListenerMessage& msg)
{
	CSLocker lock(&m_cs);
	std::map<std::string, TunnelServer*>::iterator it = m_servers.find(msg.m_id);
	if(it == m_servers.end())
	{
		return;
	}

	InitTunnel itMsg;
	itMsg.set_type(TUNNEL_RELAY);
	if(msg.m_sessionId == it->second->m_sessionIdOne)
	{
		itMsg.set_own_session_id(it->second->m_sessionIdOne);
		itMsg.set_ext_session_id(it->second->m_sessionIdTwo);
	}
	else
	{
		itMsg.set_own_session_id(it->second->m_sessionIdTwo);
		itMsg.set_ext_session_id(it->second->m_sessionIdOne);
	}
	TunnelConnectAddress* address = itMsg.mutable_address();
	address->set_ip(msg.m_ip);
	address->set_port(msg.m_port);
	InitTunnelSignal itSig(itMsg);
	onSignal(itSig);
}

void TunnelModule::onErrorRelayListener(const ListenErrorMessage& msg)
{
	CSLocker lock(&m_cs);
	std::map<std::string, TunnelServer*>::iterator it = m_servers.find(msg.m_id);
	if(it != m_servers.end())
	{
		delete it->second;
		m_servers.erase(it);
	}
}

void TunnelModule::onAddRelayServerConnector(const ConnectorMessage& msg)
{
	CSLocker lock(&m_cs);
	std::map<std::string, TunnelServer*>::iterator it = m_servers.find(msg.m_conn->GetId());
	if(it != m_servers.end())
	{
		delete it->second;
		m_servers.erase(it);
	}

	ThreadManager::GetInstance().AddThread(msg.m_conn);
	msg.m_conn->Start();
}

void TunnelModule::onErrorRelayConnect(const ConnectErrorMessage& msg)
{
}

void TunnelModule::onAddRelayConnector(const ConnectorMessage& msg)
{
//	printf("new relay connector - tunnel\n");
	IPCModule::onAddConnector(msg);
	//TODO: check another connections
}

void TunnelModule::onModuleName(const ModuleNameMessage& msg)
{
	CSLocker lock(&m_cs);
	IPCObjectName sessionId(msg.ipc_name());
	std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.find(sessionId.GetModuleNameString());
	if(it != m_tunnels.end())
	{
		delete it->second;
		m_tunnels.erase(it);
		TunnelConnected(sessionId.GetModuleNameString());
	}
}

void TunnelModule::onIPCMessage(const IPCProtoMessage& msg)
{
	printf("getting message %s\n", msg.message_name().c_str());
}

void TunnelModule::CreateLocalListenThread(const std::string& extSessionId)
{
	CSLocker lock(&m_cs);
	TunnelConnect* tunnel;
	std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.find(extSessionId);
	if(it == m_tunnels.end())
	{
		//TODO: inform about it;
		return;
	}
	tunnel = it->second;

	ListenAddress address;
	address.m_id = extSessionId;
	address.m_localIP = "";
	address.m_localPort = 0;
	address.m_connectorFactory = new IPCConnectorFactory<TunnelConnector>(m_ownSessionId);
	address.m_socketFactory = new TCPSecureSocketFactory;
	address.m_acceptCount = 1;
	ListenThread* thread = new IPCListenThread(address);
	thread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, CreatedListenerMessage, onCreatedLocalListener));
	thread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ListenErrorMessage, onErrorLocalListener));
	thread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ConnectorMessage, onAddLocalConnector));

	if(tunnel->m_localListenThread)
	{
		tunnel->m_localListenThread->Stop();
		ThreadManager::GetInstance().AddThread(tunnel->m_localListenThread);
	}

	tunnel->m_localListenThread = thread;
	thread->Start();
}

void TunnelModule::CreateLocalConnectThread(const std::string& extSessionId, const std::string& ip, int port)
{
	CSLocker lock(&m_cs);
	TunnelConnect* tunnel;
	std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.find(extSessionId);
	if(it == m_tunnels.end())
	{
		//TODO: inform about it;
		return;
	}
	tunnel = it->second;

	ConnectAddress address;
	address.m_localIP = "";
	address.m_localPort = 0;
	address.m_moduleName = address.m_id = extSessionId;
	address.m_connectorFactory = new IPCConnectorFactory<TunnelConnector>(m_ownSessionId);
	address.m_socketFactory = new TCPSecureSocketFactory;
	address.m_ip = ip;
	address.m_port = port;
	ConnectThread* thread = new ConnectThread(address);
	thread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ConnectorMessage, onAddLocalConnector));
	thread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ConnectErrorMessage, onErrorLocalConnect));
	thread->Start();
}

void TunnelModule::InitExternalConnectThread(const std::string& extSessionId, const std::string& ip, int port)
{
	CSLocker lock(&m_cs);
	TunnelConnect* tunnel;
	std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.find(extSessionId);
	if(it == m_tunnels.end())
	{
		//TODO: inform about it;
		return;
	}
	tunnel = it->second;

	ConnectAddress address;
	address.m_localIP = "";
	address.m_localPort = 0;
	address.m_moduleName = m_ownSessionId;
	address.m_id = extSessionId;
	address.m_connectorFactory = new IPCConnectorFactory<TunnelConnector>(m_ownSessionId);
	address.m_socketFactory = new UDPSocketFactory;
	address.m_ip = ip;
	address.m_port = port;
	ExternalConnectThread* thread = new ExternalConnectThread(address);
	thread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ConnectorMessage, onAddExternalConnector));
	thread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ConnectErrorMessage, onErrorExternalConnect));
	
	if(tunnel->m_externalConnectThread)
	{
		tunnel->m_externalConnectThread->Stop();
		ThreadManager::GetInstance().AddThread(tunnel->m_externalConnectThread);
	}
	tunnel->m_externalConnectThread = thread;

	thread->Start();
}

void TunnelModule::CreateExternalConnectThread(const std::string& extSessionId, const std::string& ip, int port)
{
	CSLocker lock(&m_cs);
	TunnelConnect* tunnel;
	std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.find(extSessionId);
	if(it == m_tunnels.end())
	{
		//TODO: inform about it;
		return;
	}
	tunnel = it->second;

	ConnectAddress address;
	address.m_localIP = "";
	address.m_localPort = 0;
	address.m_moduleName = address.m_id = extSessionId;
	address.m_connectorFactory = new IPCConnectorFactory<TunnelConnector>(m_ownSessionId);
	address.m_socketFactory = new UDTSecureSocketFactory;
	address.m_ip = ip;
	address.m_port = port;
	tunnel->m_externalConnectThread->ChangeConnectAddress(address);
}

void TunnelModule::CreateRelayConnectThread(const std::string& extSessionId, const std::string& ip, int port)
{
	CSLocker lock(&m_cs);
	TunnelConnect* tunnel;
	std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.find(extSessionId);
	if(it == m_tunnels.end())
	{
		//TODO: inform about it;
		return;
	}
	tunnel = it->second;

	ConnectAddress address;
	address.m_localIP = "";
	address.m_localPort = 0;
	address.m_moduleName = address.m_id = extSessionId;
	address.m_connectorFactory = new IPCConnectorFactory<TunnelConnector>(m_ownSessionId);
	address.m_socketFactory = new TCPSecureSocketFactory;
	address.m_ip = ip;
	address.m_port = port;
	ConnectThread* thread = new ConnectThread(address);
	thread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ConnectorMessage, onAddRelayConnector));
	thread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ConnectErrorMessage, onErrorRelayConnect));
	thread->Start();
}

void TunnelModule::CheckTunnels()
{
	CSLocker lock(&m_cs);
	time_t curTime;
	time(&curTime);
	for(std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.begin();
		it != m_tunnels.end();)
	{
		if(curTime - it->second->m_creationTime > 30)//TODO: 30 second ?
		{
			std::string sessionId = it->first;
			delete it->second;
			it = m_tunnels.erase(it); 
			TunnelConnectFailed(sessionId);
		}
		else
		{
			it++;
		}
	}

	for(std::map<std::string, TunnelServer*>::iterator it = m_servers.begin();
		it != m_servers.end();)
	{
		if(curTime - it->second->m_creationTime > 30)//TODO: 30 second ?
		{
			delete it->second;
			it = m_servers.erase(it); 
		}
		else
		{
			it++;
		}
	}
}
