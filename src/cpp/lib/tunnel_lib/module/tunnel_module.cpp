#include <time.h>
#include <stdio.h>
#include "tunnel_module.h"
#include "connector/tunnel_connector.h"
#include "../thread/external_listen_thread.h"
#include "../thread/external_connect_thread.h"
#include "../thread/relay_listen_thread.h"
#include "thread_lib/thread/thread_manager.h"
#include "ipc_lib/connector/ipc_connector_factory.h"
#include "connector_lib/socket/socket_factories.h"
#include "connector_lib/socket/udp_socket.h"
#include "connector_lib/ppp/ppp_library.h"
#include "common/common_func.h"
#include "common/logger.h"

extern std::vector<std::string> GetLocalIps();

const std::string TunnelModule::m_tunnelIPCName = "Tunnel";

TunnelModule::TunnelModule(const IPCObjectName& ipcName, ConnectorFactory* factory, bool isPPPListener)
	: ClientServerModule(ipcName, factory), m_pppListenThread(0)
{
	m_tunnelChecker = new TunnelCheckerThread(this);
	if(isPPPListener)
		CreatePPPListenThread();
}

TunnelModule::~TunnelModule()
{
	removeReceiver();

	m_tunnelChecker->Stop();

	if(m_pppListenThread)
	{
		m_pppListenThread->Stop();
		ThreadManager::GetInstance().AddThread(m_pppListenThread);
		m_pppListenThread = 0;
	}

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

void TunnelModule::InitNewTunnel(const std::string& extSessionId, TunnelConnector::TypeConnection type)
{
	IPCObject object(IPCObjectName(m_tunnelIPCName, extSessionId));
	if(m_modules.GetObject(object, &object))
	{
		OnTunnelConnectFailed(extSessionId);
		return;
	}
	
	LOG_INFO("Init new tunnel: from %s to %s\n", m_ownSessionId.c_str(), extSessionId.c_str());
	if (type == TunnelConnector::PPP ||
		type == TunnelConnector::UNKNOWN)
	{
		CreatePPPConnectThread(extSessionId);
	}

	PeerData peerData;
	peerData.set_one_session_id(m_ownSessionId);
	peerData.set_two_session_id(extSessionId);
	if(type == TunnelConnector::LOCAL_TCP)		
		peerData.set_type(TUNNEL_LOCAL_TCP);
	if(type == TunnelConnector::LOCAL_UDP)		
		peerData.set_type(TUNNEL_LOCAL_UDP);
	if(type == TunnelConnector::EXTERNAL)
		peerData.set_type(TUNNEL_EXTERNAL);
	if(type == TunnelConnector::RELAY)
		peerData.set_type(TUNNEL_RELAY);
	if (type != TunnelConnector::UNKNOWN &&
		type != TunnelConnector::PPP)
	{
		PeerDataSignal pdSig(peerData);
		onSignal(pdSig);
	}
	
	if(type != TunnelConnector::PPP)
	{
		InitTunnel itMsg;
		itMsg.set_own_session_id(m_ownSessionId);
		itMsg.set_ext_session_id(extSessionId);
		InitTunnelSignal itSig(itMsg);
		onSignal(itSig);
	}
}

void TunnelModule::DestroyTunnel(const std::string& extSessionId)
{
	LOG_INFO("Destroy tunnel: from %s to %s\n", m_ownSessionId.c_str(), extSessionId.c_str());
	m_manager.StopConnection(extSessionId);
}

void TunnelModule::SetTypeTunnel(const std::string& oneSessionId, const std::string& twoSessionId, TunnelConnector::TypeConnection type)
{
	LOG_INFO("Set tunnel Type: between %s %s type %d\n", oneSessionId.c_str(), twoSessionId.c_str(), type);
	PeerType peerData;
	peerData.set_one_session_id(oneSessionId);
	peerData.set_two_session_id(twoSessionId);
	bool bUpdate = false;
	if(m_typePeers.GetObject(peerData, &peerData))
		bUpdate = true;

	switch(type)
	{
	case TunnelConnector::LOCAL_TCP:
		peerData.set_type(TUNNEL_LOCAL_TCP);
		break;
	case TunnelConnector::LOCAL_UDP:
		peerData.set_type(TUNNEL_LOCAL_UDP);
		break;
	case TunnelConnector::EXTERNAL:
		peerData.set_type(TUNNEL_EXTERNAL);
		break;
	case TunnelConnector::RELAY:
		peerData.set_type(TUNNEL_RELAY);
		break;
	default:
		peerData.set_type(TUNNEL_ALL);
		break;
	}

	bUpdate ? m_typePeers.UpdateObject(peerData) : m_typePeers.AddObject(peerData);
};

void TunnelModule::OnNewConnector(Connector* connector)
{
	ClientServerConnector* clientServerConn = dynamic_cast<ClientServerConnector*>(const_cast<Connector*>(connector));
	if(clientServerConn)
	{
		ipcSubscribe(clientServerConn, SIGNAL_FUNC(this, TunnelModule, InitTunnelMessage, onInitTunnel)); //for client
		ipcSubscribe(clientServerConn, SIGNAL_FUNC(this, TunnelModule, InitTunnelSignal, onInitTunnel)); //for server
		ipcSubscribe(clientServerConn, SIGNAL_FUNC(this, TunnelModule, PeerDataSignal, onPeerData)); //for server
		ipcSubscribe(clientServerConn, SIGNAL_FUNC(this, TunnelModule, TryConnectToMessage, onTryConnectTo)); //for client
		ipcSubscribe(clientServerConn, SIGNAL_FUNC(this, TunnelModule, InitTunnelStartedMessage, onInitTunnelStarted)); //for client
	}

	TunnelConnector* tunnelConnector = dynamic_cast<TunnelConnector*>(connector);
	if(tunnelConnector)
	{
		ipcSubscribe(tunnelConnector, SIGNAL_FUNC(this, TunnelModule, ModuleNameMessage, onModuleName));
		ipcSubscribe(tunnelConnector, SIGNAL_FUNC(this, TunnelModule, TunnelConnectedMessage, onConnected));
	}
}

void TunnelModule::OnTunnelConnectFailed(const std::string& sessionId)
{
	LOG_INFO("Creation of tunnel failed: from %s to %s\n", m_ownSessionId.c_str(), sessionId.c_str());
}

void TunnelModule::OnTunnelConnected(const std::string& sessionId, TunnelConnector::TypeConnection type)
{
	LOG_INFO("Creation of tunnel successful: from %s to %s type %d\n", m_ownSessionId.c_str(), sessionId.c_str(), type);
}

void TunnelModule::FillIPCObjectList(IPCObjectListMessage& msg)
{
	std::vector<IPCObject> list = m_ipcObject.GetObjectList();
	std::vector<IPCObject>::iterator it;
	for(it = list.begin(); it != list.end(); it++)
	{
		if (it->m_ipcName.module_name() != m_clientIPCName &&
			it->m_ipcName.module_name() != m_serverIPCName &&
			it->m_ipcName.module_name() != m_tunnelIPCName)
		{
			AddIPCObject* ipcObject = const_cast<IPCObjectListMessage&>(msg).add_ipc_object();
			ipcObject->set_ip(it->m_ip);
			ipcObject->set_port(it->m_port);
			*ipcObject->mutable_ipc_name() = it->m_ipcName;
		}
	}
}

void TunnelModule::onPeerData(const PeerDataSignal& msg)
{
	LOG_INFO("Set tunnel Type: between %s %s type %d\n", const_cast<PeerDataSignal&>(msg).one_session_id().c_str(),
														const_cast<PeerDataSignal&>(msg).two_session_id().c_str(),
														const_cast<PeerDataSignal&>(msg).type());
	PeerType peerData(msg);
	if(!m_typePeers.AddObject(peerData))
	{
		m_typePeers.UpdateObject(peerData);
	}
}

void TunnelModule::onInitTunnel(const InitTunnelMessage& msg)
{
	if(!msg.has_type())
	{
		return;
	}

	if(msg.type() == TUNNEL_LOCAL_TCP)
	{
		CreateLocalListenThread(msg.ext_session_id());
	}
	else if(msg.type() == TUNNEL_LOCAL_UDP)
	{
		CreateLocalUDPSocket(msg.ext_session_id());
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
	LOG_INFO("Send InitTunnelStarted message: from %s to %s\n", msg.own_session_id().c_str(), msg.ext_session_id().c_str());
	InitTunnelStarted itsMsg;
	itsMsg.set_own_session_id(msg.own_session_id());
	itsMsg.set_ext_session_id(msg.ext_session_id());
	InitTunnelStartedSignal itrSig(itsMsg);
	onSignal(itrSig);
	itrSig.set_own_session_id(msg.ext_session_id());
	itrSig.set_ext_session_id(msg.own_session_id());
	onSignal(itrSig);

	TunnelType type;
	PeerType peerType;
	peerType.set_one_session_id(msg.own_session_id());
	peerType.set_two_session_id(msg.ext_session_id());
	if(m_typePeers.GetObject(peerType, &peerType) && peerType.has_type())
		type = peerType.type();
	else
		type = TUNNEL_ALL;

	//--------local connection---------------------
	if(type == TUNNEL_LOCAL_TCP || type == TUNNEL_ALL)
	{
		LOG_INFO("Send InitTunnel message: from %s to %s, type %d\n", msg.own_session_id().c_str(), msg.ext_session_id().c_str(), TUNNEL_LOCAL_TCP);
		InitTunnelSignal itMsg(msg);
		itMsg.set_type(TUNNEL_LOCAL_TCP);
		onSignal(itMsg);
		itMsg.set_own_session_id(msg.ext_session_id());
		itMsg.set_ext_session_id(msg.own_session_id());
		onSignal(itMsg);
	}
	
	if(type == TUNNEL_LOCAL_UDP)
	{
		LOG_INFO("Send InitTunnel message: from %s to %s, type %d\n", msg.own_session_id().c_str(), msg.ext_session_id().c_str(), TUNNEL_LOCAL_UDP);
		InitTunnelSignal itMsg(msg);
		itMsg.set_type(TUNNEL_LOCAL_UDP);
		onSignal(itMsg);
		itMsg.set_own_session_id(msg.ext_session_id());
		itMsg.set_ext_session_id(msg.own_session_id());
		onSignal(itMsg);
	}
	
	//----------------external connection-----------
	if(type == TUNNEL_EXTERNAL || type == TUNNEL_ALL)
	{
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
		SignalOwner* threadOwner = dynamic_cast<SignalOwner*>(external->m_thread);
		threadOwner->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, CreatedServerListenerMessage, onCreatedExternalListener));
		threadOwner->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, GotExternalAddressMessage, onGotExternalAddress));
		threadOwner->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ListenErrorMessage, onErrorExternalListener));
		external->m_thread->Start();
		LOG_INFO("Start external listen thread: from %s to %s, id - %s\n", msg.own_session_id().c_str(), msg.ext_session_id().c_str(), address.m_id.c_str());
	}
	
	//-----------relay connection-------------------
	if(type == TUNNEL_RELAY || type == TUNNEL_ALL)
	{
		TunnelServerListenAddress address;
		address.m_localIP = "";
		address.m_localPort = 0;
		address.m_sessionIdOne = msg.own_session_id();
		address.m_sessionIdTwo = msg.ext_session_id();
		address.m_id = CreateGUID();
		address.m_socketFactory = 0;
		TunnelServer* relay = new TunnelServer(msg.own_session_id(), msg.ext_session_id());
		relay->m_thread = new RelayListenThread(address);
		m_servers.insert(std::make_pair(address.m_id, relay));
		SignalOwner* threadOwner = dynamic_cast<SignalOwner*>(relay->m_thread);
		threadOwner->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, CreatedServerListenerMessage, onCreatedRelayListener));
		threadOwner->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ListenErrorMessage, onErrorRelayListener));
		threadOwner->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ConnectorMessage, onAddRelayServerConnector));
		relay->m_thread->Start();
		LOG_INFO("Start relay listen thread: from %s to %s, id - %s\n", msg.own_session_id().c_str(), msg.ext_session_id().c_str(), address.m_id.c_str());
	}
}

void TunnelModule::onTryConnectTo(const TryConnectToMessage& msg)
{
	if(msg.type() == TUNNEL_LOCAL_TCP || msg.type() == TUNNEL_LOCAL_UDP)
	{
		for(int i = 0; i < msg.adresses_size(); i++)
		{
			CreateLocalConnectThread(msg.ext_session_id(), msg.adresses(i).ip(), msg.adresses(i).port(), msg.type() == TUNNEL_LOCAL_TCP);
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
	LOG_INFO("InitTunnelStarted Message: from %s to %s\n", msg.own_session_id().c_str(), msg.ext_session_id().c_str());
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
	LOG_INFO("Send TryConnectTo message: from %s to %s, type %d\n", m_ownSessionId.c_str(), msg.m_id.c_str(), TUNNEL_LOCAL_TCP);
	TryConnectTo tctMsg;
	tctMsg.set_type(TUNNEL_LOCAL_TCP);
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
	LOG_INFO("Error in local listen thread(tunnel connection): from %s to %s, type %d\n", m_ownSessionId.c_str(), msg.m_id.c_str(), TUNNEL_LOCAL_TCP);
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
	LOG_INFO("Connection failed(tunnel connection): from %s to %s, type %d\n", m_ownSessionId.c_str(), msg.m_moduleName.c_str(), TUNNEL_LOCAL_TCP);
}

void TunnelModule::onAddLocalTCPConnector(const ConnectorMessage& msg)
{
	TunnelConnector* connector = dynamic_cast<TunnelConnector*>(msg.m_conn);
	if(connector)
	{
		connector->SetTypeConnection(TunnelConnector::LOCAL_TCP);
	}
	IPCModule::onAddConnector(msg);
}

void TunnelModule::onAddLocalUDPConnector(const ConnectorMessage& msg)
{
	{
		CSLocker lock(&m_cs);
		IPCObjectName sessionId(msg.m_conn->GetId());
		std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.find(sessionId.module_name());
		if(it != m_tunnels.end())
		{
			it->second->m_localUdpSocket = 0;
		}
	}

	TunnelConnector* connector = dynamic_cast<TunnelConnector*>(msg.m_conn);
	if(connector)
	{
		connector->SetTypeConnection(TunnelConnector::LOCAL_UDP);

	}
	IPCModule::onAddConnector(msg);
}

void TunnelModule::onErrorExternalConnect(const ConnectErrorMessage& msg)
{
	LOG_INFO("Connection failed(tunnel connection): from %s to %s, type %d\n", m_ownSessionId.c_str(), msg.m_moduleName.c_str(), TUNNEL_EXTERNAL);
	CSLocker lock(&m_cs);
	std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.find(msg.m_moduleName);
	if(it != m_tunnels.end())
	{
		if(it->second->m_externalConnectThread)
		{
			it->second->m_externalConnectThread->Stop();
			ThreadManager::GetInstance().AddThread(it->second->m_externalConnectThread);
		}
		it->second->m_externalConnectThread = 0;
	}
}

void TunnelModule::onAddExternalConnector(const ConnectorMessage& msg)
{
	TunnelConnector* connector = dynamic_cast<TunnelConnector*>(msg.m_conn);
	if(connector)
	{
		connector->SetTypeConnection(TunnelConnector::EXTERNAL);
	}
	IPCModule::onAddConnector(msg);
}

void TunnelModule::onCreatedExternalListener(const CreatedServerListenerMessage& msg)
{
	CSLocker lock(&m_cs);
	std::map<std::string, TunnelServer*>::iterator it = m_servers.find(msg.m_id);
	if(it == m_servers.end())
	{
		LOG_WARNING("Not found tunnel: id - %s\n", msg.m_id.c_str());
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
	LOG_INFO("Send InitTunnel message: from %s to %s, type %d\n", it->second->m_sessionIdOne.c_str(), it->second->m_sessionIdTwo.c_str(), TUNNEL_EXTERNAL);
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
		LOG_WARNING("Not found tunnel: id - %s\n", msg.m_id.c_str());
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
	LOG_INFO("Send TryConnectTo message: from %s to %s, type %d\n", it->second->m_sessionIdOne.c_str(), it->second->m_sessionIdTwo.c_str(), TUNNEL_EXTERNAL);
	TunnelConnectAddress* address = tctMsg.add_adresses();
	address->set_ip(msg.m_ip);
	address->set_port(msg.m_port);
	TryConnectToSignal tctSig(tctMsg);
	onSignal(tctSig);
}

void TunnelModule::onErrorExternalListener(const ListenErrorMessage& msg)
{
	LOG_INFO("Error in external listen thread(tunnel connection): id - %s\n", msg.m_id.c_str());
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
		LOG_WARNING("Not found tunnel: id - %s\n", msg.m_id.c_str());
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
	LOG_INFO("Send InitTunnel message: from %s to %s, type %d\n", it->second->m_sessionIdOne.c_str(), it->second->m_sessionIdTwo.c_str(), TUNNEL_RELAY);
	TunnelConnectAddress* address = itMsg.mutable_address();
	address->set_ip(msg.m_ip);
	address->set_port(msg.m_port);
	InitTunnelSignal itSig(itMsg);
	onSignal(itSig);
}

void TunnelModule::onErrorRelayListener(const ListenErrorMessage& msg)
{
	LOG_INFO("Error in relay listen thread(tunnel connection): id - %s\n", msg.m_id.c_str());
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
	LOG_INFO("Connection failed(tunnel connection): from %s to %s, type %d\n", m_ownSessionId.c_str(), msg.m_moduleName.c_str(), TUNNEL_RELAY);
}

void TunnelModule::onAddRelayConnector(const ConnectorMessage& msg)
{
	TunnelConnector* connector = dynamic_cast<TunnelConnector*>(msg.m_conn);
	if(connector)
	{
		connector->SetTypeConnection(TunnelConnector::RELAY);
	}

	IPCModule::onAddConnector(msg);
}

void TunnelModule::onCreatedPPPListener(const CreatedListenerMessage& msg)
{
}

void TunnelModule::onErrorPPPListener(const ListenErrorMessage& msg)
{
}

void TunnelModule::onErrorPPPConnect(const ConnectErrorMessage& msg)
{
}

void TunnelModule::onAddPPPConnector(const ConnectorMessage& msg)
{
	TunnelConnector* connector = dynamic_cast<TunnelConnector*>(msg.m_conn);
	if(connector)
	{
		connector->SetTypeConnection(TunnelConnector::PPP);
	}

	IPCModule::onAddConnector(msg);
}

void TunnelModule::onModuleName(const ModuleNameMessage& msg)
{
	CSLocker lock(&m_cs);
	IPCObjectName sessionId(msg.ipc_name());
	std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.find(sessionId.host_name());
	if(it != m_tunnels.end())
	{
		delete it->second;
		m_tunnels.erase(it);
	}
}

void TunnelModule::onConnected(const TunnelConnectedMessage& msg)
{
	OnTunnelConnected(msg.m_id, msg.m_type);
}

void TunnelModule::CreateLocalListenThread(const std::string& extSessionId)
{
	LOG_INFO("Start listen thread(tunnel connection): from %s to %s, type %d\n", m_ownSessionId.c_str(), extSessionId.c_str(), TUNNEL_LOCAL_TCP);
	CSLocker lock(&m_cs);
	TunnelConnect* tunnel;
	std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.find(extSessionId);
	if(it == m_tunnels.end())
	{
		LOG_WARNING("Not found tunnel: from %s to %s\n", m_ownSessionId.c_str(), extSessionId.c_str());
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
	thread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ListenErrorMessage, onErrorLocalListener));
	thread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, CreatedListenerMessage, onCreatedLocalListener));
	thread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ConnectorMessage, onAddLocalTCPConnector));

	if(tunnel->m_localListenThread)
	{
		tunnel->m_localListenThread->Stop();
		ThreadManager::GetInstance().AddThread(tunnel->m_localListenThread);
	}

	tunnel->m_localListenThread = thread;
	thread->Start();
}

void TunnelModule::CreateLocalUDPSocket(const std::string& extSessionId)
{
	LOG_INFO("Create udp socket(tunnel connection): from %s to %s, type %d\n", m_ownSessionId.c_str(), extSessionId.c_str(), TUNNEL_LOCAL_UDP);
	CSLocker lock(&m_cs);
	TunnelConnect* tunnel;
	std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.find(extSessionId);
	if(it == m_tunnels.end())
	{
		LOG_WARNING("Not found tunnel: from %s to %s\n", m_ownSessionId.c_str(), extSessionId.c_str());
		return;
	}

	tunnel = it->second;
	tunnel->m_localUdpSocket = new UDPSocket;
	if(!tunnel->m_localUdpSocket->Bind("", 0))
	{
		LOG_WARNING("Failed bind upd socket(tunnel connection): from %s to %s\n", m_ownSessionId.c_str(), extSessionId.c_str());
		return;
	}

	int port;
	std::string ip;
	tunnel->m_localUdpSocket->GetIPPort(ip, port);

	TryConnectTo tctMsg;
	tctMsg.set_type(TUNNEL_LOCAL_UDP);
	tctMsg.set_own_session_id(m_ownSessionId);
	tctMsg.set_ext_session_id(extSessionId);
	std::vector<std::string> ips = GetLocalIps();
	for(size_t i = 0; i < ips.size(); i++)
	{
		TunnelConnectAddress* address = tctMsg.add_adresses();
		address->set_ip(ips[i]);
		address->set_port(port);
	}
	
	LOG_INFO("Send TryConnectTo message: from %s to %s, type %d\n", m_ownSessionId.c_str(), extSessionId.c_str(), TUNNEL_LOCAL_UDP);
	TryConnectToSignal tctSig(tctMsg);
	onSignal(tctSig);
}

void TunnelModule::CreateLocalConnectThread(const std::string& extSessionId, const std::string& ip, int port, bool isTCP)
{
	LOG_INFO("Try creation tunnel: from %s to %s, type %d\n", m_ownSessionId.c_str(), extSessionId.c_str(), isTCP ? TUNNEL_LOCAL_TCP : TUNNEL_LOCAL_UDP);
	CSLocker lock(&m_cs);
	TunnelConnect* tunnel;
	std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.find(extSessionId);
	if(it == m_tunnels.end())
	{
		LOG_WARNING("Not found tunnel: from %s to %s\n", m_ownSessionId.c_str(), extSessionId.c_str());
		return;
	}
	tunnel = it->second;
	
	SocketFactory* factory;
	if(m_isUseProxy && isTCP)
		factory = new TCPSecureProxySocketFactory(m_proxyIp, m_proxyPort, m_proxyUserPassword.m_userName, m_proxyUserPassword.m_password);
	else if(isTCP)
		factory = new TCPSecureSocketFactory;
	else
	{
		UDTSecureSocketFactory *udtFactory = new UDTSecureSocketFactory;
		udtFactory->SetUdpSocket(tunnel->m_localUdpSocket->GetSocket());
		factory = udtFactory;
	}
	ConnectAddress address;
	address.m_localIP = "";
	address.m_localPort = 0;
	address.m_moduleName = address.m_id = extSessionId;
	address.m_connectorFactory = new IPCConnectorFactory<TunnelConnector>(m_ownSessionId);
	address.m_socketFactory = factory;
	address.m_ip = ip;
	address.m_port = port;
	ConnectThread* thread = new ConnectThread(address);
	if(isTCP)
		thread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ConnectorMessage, onAddLocalTCPConnector));
	else
		thread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ConnectorMessage, onAddLocalUDPConnector));
	thread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ConnectErrorMessage, onErrorLocalConnect));
	thread->Start();
}

void TunnelModule::InitExternalConnectThread(const std::string& extSessionId, const std::string& ip, int port)
{
	LOG_INFO("Try creation tunnel: from %s to %s, type %d (Init)\n", m_ownSessionId.c_str(), extSessionId.c_str(), TUNNEL_EXTERNAL);
	CSLocker lock(&m_cs);
	TunnelConnect* tunnel;
	std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.find(extSessionId);
	if(it == m_tunnels.end())
	{
		LOG_WARNING("Not found tunnel: from %s to %s\n", m_ownSessionId.c_str(), extSessionId.c_str());
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
	LOG_INFO("Try creation tunnel: from %s to %s, type %d (Connect)\n", m_ownSessionId.c_str(), extSessionId.c_str(), TUNNEL_EXTERNAL);
	CSLocker lock(&m_cs);
	TunnelConnect* tunnel;
	std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.find(extSessionId);
	if(it == m_tunnels.end())
	{
		LOG_WARNING("Not found tunnel: from %s to %s\n", m_ownSessionId.c_str(), extSessionId.c_str());
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
	LOG_INFO("Try creation tunnel: from %s to %s, type %d\n", m_ownSessionId.c_str(), extSessionId.c_str(), TUNNEL_RELAY);
	CSLocker lock(&m_cs);
	TunnelConnect* tunnel;
	std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.find(extSessionId);
	if(it == m_tunnels.end())
	{
		LOG_WARNING("Not found tunnel: from %s to %s\n", m_ownSessionId.c_str(), extSessionId.c_str());
		return;
	}
	tunnel = it->second;
	
	SocketFactory* factory;
	if(m_isUseProxy)
		factory = new TCPSecureProxySocketFactory(m_proxyIp, m_proxyPort, m_proxyUserPassword.m_userName, m_proxyUserPassword.m_password);
	else
		factory = new TCPSecureSocketFactory;

	ConnectAddress address;
	address.m_localIP = "";
	address.m_localPort = 0;
	address.m_moduleName = address.m_id = extSessionId;
	address.m_connectorFactory = new IPCConnectorFactory<TunnelConnector>(m_ownSessionId);
	address.m_socketFactory = factory;
	address.m_ip = ip;
	address.m_port = port;
	ConnectThread* thread = new ConnectThread(address);
	thread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ConnectorMessage, onAddRelayConnector));
	thread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ConnectErrorMessage, onErrorRelayConnect));
	thread->Start();
}

void TunnelModule::CreatePPPConnectThread(const std::string& extSessionId)
{
#ifdef WIN32
	CSLocker lock(&m_cs);
	TunnelConnect* tunnel;
	std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.find(extSessionId);
	if(it == m_tunnels.end())
	{
		tunnel = new TunnelConnect(extSessionId);
		m_tunnels.insert(std::make_pair(extSessionId, tunnel));
	}
	
	ConnectAddress address;
	address.m_localIP = "";
	address.m_localPort = 0;
	address.m_moduleName = address.m_id = extSessionId;
	address.m_connectorFactory = new IPCConnectorFactory<TunnelConnector>(PPPLibrary::GetInstance().GetOwnId());
	address.m_socketFactory = new PPPSecureSocketFactory(extSessionId);
	address.m_ip = "";
	address.m_port = 0;
	ConnectThread* thread = new ConnectThread(address);
	thread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ConnectorMessage, onAddPPPConnector));
	thread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ConnectErrorMessage, onErrorPPPConnect));
	thread->Start();
#endif/*WIN32*/
}

void TunnelModule::CreatePPPListenThread()
{
#ifdef WIN32
	if(m_pppListenThread)
	{
		m_pppListenThread->Stop();
		ThreadManager::GetInstance().AddThread(m_pppListenThread);
		m_pppListenThread = 0;
	}

	ListenAddress address;
	address.m_id = m_clientIPCName;
	address.m_localIP = "";
	address.m_localPort = 0;
	address.m_connectorFactory = new IPCConnectorFactory<TunnelConnector>(PPPLibrary::GetInstance().GetOwnId());
	address.m_socketFactory = new PPPSecureSocketFactory("");
	address.m_acceptCount = -1;
	m_pppListenThread = new IPCListenThread(address);
	m_pppListenThread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, CreatedListenerMessage, onCreatedPPPListener));
	m_pppListenThread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ListenErrorMessage, onErrorPPPListener));
	m_pppListenThread->addSubscriber(this, SIGNAL_FUNC(this, TunnelModule, ConnectorMessage, onAddPPPConnector));
	m_pppListenThread->Start();
#endif/*WIN32*/
}

void TunnelModule::CheckTunnels()
{
	CSLocker lock(&m_cs);
	time_t curTime;
	time(&curTime);
	for(std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.begin();
		it != m_tunnels.end();)
	{
		if(curTime - it->second->m_creationTime > 60)//TODO: 60 second ?
		{
			std::string sessionId = it->first;
			delete it->second;
			m_tunnels.erase(it++); 
			OnTunnelConnectFailed(sessionId);
		}
		else
		{
			it++;
		}
	}

	for(std::map<std::string, TunnelServer*>::iterator it = m_servers.begin();
		it != m_servers.end();)
	{
		if(curTime - it->second->m_creationTime > 60)//TODO: 60 second ?
		{
			delete it->second;
			m_servers.erase(it++); 
		}
		else
		{
			it++;
		}
	}
}
