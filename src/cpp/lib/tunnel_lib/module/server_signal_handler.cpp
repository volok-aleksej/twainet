#include "server_signal_handler.h"
#include "tunnel_module.h"
#include "thread/tunnel_server_listen_address.h"
#include "thread/external_listen_thread.h"
#include "thread/relay_listen_thread.h"
#include "connector_lib/socket/socket_factories.h"
#include "thread_lib/thread/thread_manager.h"
#include "common/common_func.h"
#include "common/logger.h"

extern std::vector<std::string> GetLocalIps();

ServerSignalHandler::ServerSignalHandler(TunnelModule* module)
: m_module(module)
{
}

ServerSignalHandler::~ServerSignalHandler()
{
	removeReceiver();
}
	
void ServerSignalHandler::onPeerData(const PeerDataSignal& msg)
{
	LOG_INFO("Set tunnel Type: between %s %s type %d\n", const_cast<PeerDataSignal&>(msg).one_session_id().c_str(),
							     const_cast<PeerDataSignal&>(msg).two_session_id().c_str(),
							     const_cast<PeerDataSignal&>(msg).type());
	PeerType peerData(msg);
	if(!m_module->m_typePeers.AddObject(peerData))
	{
		m_module->m_typePeers.UpdateObject(peerData);
	}
}

void ServerSignalHandler::onInitTunnel(const InitTunnelSignal& msg)
{
	TunnelType type;
	PeerType peerType;
	peerType.set_one_session_id(msg.own_session_id());
	peerType.set_two_session_id(msg.ext_session_id());
	if(m_module->m_typePeers.GetObject(peerType, &peerType) && peerType.has_type())
		type = peerType.type();
	else
		type = TUNNEL_ALL;
	
	peerType.set_type(type);
	TunnelStep step(peerType);
	if(!m_module->m_tunnelsStep.AddObject(step))
	{
		return;
	}
  
	LOG_INFO("Send InitTunnelStarted message: from %s to %s\n", msg.own_session_id().c_str(), msg.ext_session_id().c_str());
	InitTunnelStarted itsMsg;
	itsMsg.set_own_session_id(msg.own_session_id());
	itsMsg.set_ext_session_id(msg.ext_session_id());
	InitTunnelStartedSignal itrSig(itsMsg);
	m_module->onSignal(itrSig);
	itrSig.set_own_session_id(msg.ext_session_id());
	itrSig.set_ext_session_id(msg.own_session_id());
	m_module->onSignal(itrSig);

	//--------local connection---------------------
	if(type == TUNNEL_LOCAL_TCP || type == TUNNEL_ALL)
	{
		LOG_INFO("Send InitTunnel message: from %s to %s, type %d\n", msg.own_session_id().c_str(), msg.ext_session_id().c_str(), TUNNEL_LOCAL_TCP);
		InitTunnelSignal itMsg(msg);
		itMsg.set_type(TUNNEL_LOCAL_TCP);
		m_module->onSignal(itMsg);
		itMsg.set_own_session_id(msg.ext_session_id());
		itMsg.set_ext_session_id(msg.own_session_id());
		m_module->onSignal(itMsg);
	}
	
	if(type == TUNNEL_LOCAL_UDP)
	{
		LOG_INFO("Send InitTunnel message: from %s to %s, type %d\n", msg.own_session_id().c_str(), msg.ext_session_id().c_str(), TUNNEL_LOCAL_UDP);
		InitTunnelSignal itMsg(msg);
		itMsg.set_type(TUNNEL_LOCAL_UDP);
		m_module->onSignal(itMsg);
		itMsg.set_own_session_id(msg.ext_session_id());
		itMsg.set_ext_session_id(msg.own_session_id());
		m_module->onSignal(itMsg);
	}
	
	//----------------external connection-----------
	if(type == TUNNEL_EXTERNAL || type == TUNNEL_ALL)
	{
		TunnelServerListenAddress address;
		address.m_localIP = msg.has_address() ? msg.address().ip() : "";
		address.m_localPort = msg.has_address() ? msg.address().port() : 0;
		address.m_sessionIdOne = msg.own_session_id();
		address.m_sessionIdTwo = msg.ext_session_id();
		address.m_id = CreateGUID();
		address.m_socketFactory = new UDPSocketFactory;
		TunnelServer* external = new TunnelServer(msg.own_session_id(), msg.ext_session_id());
		external->m_thread = new ExternalListenThread(address);
		m_module->m_servers.insert(std::make_pair(address.m_id, external));
		SignalOwner* threadOwner = dynamic_cast<SignalOwner*>(external->m_thread);
		threadOwner->addSubscriber(this, SIGNAL_FUNC(this, ServerSignalHandler, CreatedServerListenerMessage, onCreatedExternalListener));
		threadOwner->addSubscriber(this, SIGNAL_FUNC(this, ServerSignalHandler, GotExternalAddressMessage, onGotExternalAddress));
		threadOwner->addSubscriber(this, SIGNAL_FUNC(this, ServerSignalHandler, ListenErrorMessage, onErrorExternalListener));
		external->m_thread->Start();
		LOG_INFO("Start external listen thread: from %s to %s, id - %s\n", msg.own_session_id().c_str(), msg.ext_session_id().c_str(), address.m_id.c_str());
	}
	
	//-----------relay connection-------------------
	if(type == TUNNEL_RELAY_TCP || type == TUNNEL_ALL)
	{
		TunnelServerListenAddress address;
		address.m_localIP = msg.has_address() ? msg.address().ip() : "";
		address.m_localPort = msg.has_address() ? msg.address().port() : 0;
		address.m_sessionIdOne = msg.own_session_id();
		address.m_sessionIdTwo = msg.ext_session_id();
		address.m_id = CreateGUID();
		address.m_socketFactory = new TCPSecureSocketFactory;
		TunnelServer* relay = new TunnelServer(msg.own_session_id(), msg.ext_session_id());
		relay->m_thread = new RelayListenThread(address);
		m_module->m_servers.insert(std::make_pair(address.m_id, relay));
		SignalOwner* threadOwner = dynamic_cast<SignalOwner*>(relay->m_thread);
		threadOwner->addSubscriber(this, SIGNAL_FUNC(this, ServerSignalHandler, CreatedServerListenerMessage, onCreatedRelayTCPListener));
		threadOwner->addSubscriber(this, SIGNAL_FUNC(this, ServerSignalHandler, ListenErrorMessage, onErrorRelayListener));
		threadOwner->addSubscriber(this, SIGNAL_FUNC(this, ServerSignalHandler, ConnectorMessage, onAddRelayServerConnector));
		relay->m_thread->Start();
		LOG_INFO("Start relay tcp listen thread: from %s to %s, id - %s\n", msg.own_session_id().c_str(), msg.ext_session_id().c_str(), address.m_id.c_str());
	}

	if(type == TUNNEL_RELAY_UDP)
	{
		TunnelServerListenAddress address;
		address.m_localIP = msg.has_address() ? msg.address().ip() : GetLocalIps()[0]/*TODO: use external ip*/;
		address.m_localPort = msg.has_address() ? msg.address().port() : 0;
		address.m_sessionIdOne = msg.own_session_id();
		address.m_sessionIdTwo = msg.ext_session_id();
		address.m_id = CreateGUID();
		address.m_socketFactory = new UDTSecureSocketFactory;
		TunnelServer* relay = new TunnelServer(msg.own_session_id(), msg.ext_session_id());
		relay->m_thread = new RelayListenThread(address);
		m_module->m_servers.insert(std::make_pair(address.m_id, relay));
		SignalOwner* threadOwner = dynamic_cast<SignalOwner*>(relay->m_thread);
		threadOwner->addSubscriber(this, SIGNAL_FUNC(this, ServerSignalHandler, CreatedServerListenerMessage, onCreatedRelayUDPListener));
		threadOwner->addSubscriber(this, SIGNAL_FUNC(this, ServerSignalHandler, ListenErrorMessage, onErrorRelayListener));
		threadOwner->addSubscriber(this, SIGNAL_FUNC(this, ServerSignalHandler, ConnectorMessage, onAddRelayServerConnector));
		relay->m_thread->Start();
		LOG_INFO("Start relay udp listen thread: from %s to %s, id - %s\n", msg.own_session_id().c_str(), msg.ext_session_id().c_str(), address.m_id.c_str());
	}
}

void ServerSignalHandler::onInitTunnelComplete(const InitTunnelCompleteMessage& msg)
{
	TunnelStep step;
	step.set_one_session_id(msg.own_session_id());
	step.set_two_session_id(msg.ext_session_id());
	m_module->m_tunnelsStep.RemoveObject(step);
}

void ServerSignalHandler::onCreatedExternalListener(const CreatedServerListenerMessage& msg)
{
	CSLocker lock(&m_module->m_cs);
	std::map<std::string, TunnelServer*>::iterator it = m_module->m_servers.find(msg.m_id);
	if(it == m_module->m_servers.end())
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
	m_module->onSignal(itSig);

}

void ServerSignalHandler::onGotExternalAddress(const GotExternalAddressMessage& msg)
{
	CSLocker lock(&m_module->m_cs);
	std::map<std::string, TunnelServer*>::iterator it = m_module->m_servers.find(msg.m_id);
	if(it == m_module->m_servers.end())
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
	m_module->onSignal(tctSig);
}

void ServerSignalHandler::onErrorExternalListener(const ListenErrorMessage& msg)
{
	LOG_INFO("Error in external listen thread(tunnel connection): id - %s\n", msg.m_id.c_str());
	CSLocker lock(&m_module->m_cs);
	std::map<std::string, TunnelServer*>::iterator it = m_module->m_servers.find(msg.m_id);
	if(it != m_module->m_servers.end())
	{
		delete it->second;
		m_module->m_servers.erase(it);
	}	
}

void ServerSignalHandler::onCreatedRelayTCPListener(const CreatedServerListenerMessage& msg)
{
	CSLocker lock(&m_module->m_cs);
	std::map<std::string, TunnelServer*>::iterator it = m_module->m_servers.find(msg.m_id);
	if(it == m_module->m_servers.end())
	{
		LOG_WARNING("Not found tunnel: id - %s\n", msg.m_id.c_str());
		return;
	}

	InitTunnel itMsg;
	itMsg.set_type(TUNNEL_RELAY_TCP);
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
	LOG_INFO("Send InitTunnel message: from %s to %s, type %d\n", it->second->m_sessionIdOne.c_str(), it->second->m_sessionIdTwo.c_str(), TUNNEL_RELAY_TCP);
	TunnelConnectAddress* address = itMsg.mutable_address();
	address->set_ip(msg.m_ip);
	address->set_port(msg.m_port);
	InitTunnelSignal itSig(itMsg);
	m_module->onSignal(itSig);
}

void ServerSignalHandler::onCreatedRelayUDPListener(const CreatedServerListenerMessage& msg)
{
	CSLocker lock(&m_module->m_cs);
	std::map<std::string, TunnelServer*>::iterator it = m_module->m_servers.find(msg.m_id);
	if(it == m_module->m_servers.end())
	{
		LOG_WARNING("Not found tunnel: id - %s\n", msg.m_id.c_str());
		return;
	}

	InitTunnel itMsg;
	itMsg.set_type(TUNNEL_RELAY_UDP);
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
	LOG_INFO("Send InitTunnel message: from %s to %s, type %d\n", it->second->m_sessionIdOne.c_str(), it->second->m_sessionIdTwo.c_str(), TUNNEL_RELAY_UDP);
	TunnelConnectAddress* address = itMsg.mutable_address();
	address->set_ip(msg.m_ip);
	address->set_port(msg.m_port);
	InitTunnelSignal itSig(itMsg);
	m_module->onSignal(itSig);
}

void ServerSignalHandler::onErrorRelayListener(const ListenErrorMessage& msg)
{
	LOG_INFO("Error in relay listen thread(tunnel connection): id - %s\n", msg.m_id.c_str());
	CSLocker lock(&m_module->m_cs);
	std::map<std::string, TunnelServer*>::iterator it = m_module->m_servers.find(msg.m_id);
	if(it != m_module->m_servers.end())
	{
		delete it->second;
		m_module->m_servers.erase(it);
	}
}

void ServerSignalHandler::onAddRelayServerConnector(const ConnectorMessage& msg)
{
	CSLocker lock(&m_module->m_cs);
	std::map<std::string, TunnelServer*>::iterator it = m_module->m_servers.find(msg.m_conn->GetId());
	if(it != m_module->m_servers.end())
	{
		delete it->second;
		m_module->m_servers.erase(it);
	}

	ThreadManager::GetInstance().AddThread(msg.m_conn);
	msg.m_conn->Start();
}
