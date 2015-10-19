#include "client_signal_handler.h"
#include "tunnel_module.h"
#include "thread_lib/thread/thread_manager.h"
#include "thread/external_connect_thread.h"
#include "common/logger.h"

extern std::vector<std::string> GetLocalIps(int ipv);

ClientSignalHandler::ClientSignalHandler(TunnelModule* module)
: m_module(module)
{
}

ClientSignalHandler::~ClientSignalHandler()
{
	removeReceiver();
}
	
void ClientSignalHandler::onInitTunnel(const InitTunnelMessage& msg)
{
	if(!msg.has_type())
	{
		return;
	}

	if(msg.type() == TUNNEL_LOCAL_TCP)
	{
		m_module->CreateLocalListenThread(msg.ext_session_id());
	}
	else if(msg.type() == TUNNEL_LOCAL_UDP)
	{
		m_module->CreateLocalUDPSocket(msg.ext_session_id());
	}
	else if(msg.type() == TUNNEL_EXTERNAL)
	{
		m_module->InitExternalConnectThread(msg.ext_session_id(), /*msg.address().ip()*/m_module->m_ip, msg.address().port());
	}
	else if(msg.type() == TUNNEL_RELAY_TCP)
	{
		m_module->CreateRelayConnectThread(msg.ext_session_id(), /*msg.address().ip()*/m_module->m_ip, msg.address().port(), true);
	}
	else if(msg.type() == TUNNEL_RELAY_UDP)
	{
		m_module->CreateRelayConnectThread(msg.ext_session_id(), /*msg.address().ip()*/m_module->m_ip, msg.address().port(), false);
	}
}

void ClientSignalHandler::onTryConnectTo(const TryConnectToMessage& msg)
{
	if(msg.type() == TUNNEL_LOCAL_TCP || msg.type() == TUNNEL_LOCAL_UDP)
	{
		for(int i = 0; i < msg.adresses_size(); i++)
		{
			m_module->CreateLocalConnectThread(msg.ext_session_id(), msg.adresses(i).ip(), msg.adresses(i).port(), msg.type() == TUNNEL_LOCAL_TCP);
		}
	}
	else if(msg.type() == TUNNEL_EXTERNAL)
	{
		for(int i = 0; i < msg.adresses_size(); i++)
		{
			m_module->CreateExternalConnectThread(msg.ext_session_id(), msg.adresses(i).ip(), msg.adresses(i).port());
		}
	}
}

void ClientSignalHandler::onInitTunnelStarted(const InitTunnelStartedMessage& msg)
{
	LOG_INFO("InitTunnelStarted Message: from %s to %s\n", msg.own_session_id().c_str(), msg.ext_session_id().c_str());
	CSLocker lock(&m_module->m_cs);
	TunnelConnect* tunnel;
	std::map<std::string, TunnelConnect*>::iterator it = m_module->m_tunnels.find(msg.ext_session_id());
	if(it == m_module->m_tunnels.end())
	{
		tunnel = new TunnelConnect(msg.ext_session_id());
		m_module->m_tunnels.insert(std::make_pair(msg.ext_session_id(), tunnel));
	}
}

void ClientSignalHandler::onCreatedLocalListener(const CreatedListenerMessage& msg)
{
	LOG_INFO("Send TryConnectTo message: from %s to %s, type %d\n", m_module->m_ownSessionId.c_str(), msg.m_id.c_str(), TUNNEL_LOCAL_TCP);
	TryConnectTo tctMsg;
	tctMsg.set_type(TUNNEL_LOCAL_TCP);
	tctMsg.set_own_session_id(m_module->m_ownSessionId);
	tctMsg.set_ext_session_id(msg.m_id);
	std::vector<std::string> ips = GetLocalIps(m_module->m_ipv);
	for(size_t i = 0; i < ips.size(); i++)
	{
		TunnelConnectAddress* address = tctMsg.add_adresses();
		address->set_ip(ips[i]);
		address->set_port(msg.m_port);
	}

	TryConnectToSignal tctSig(tctMsg);
	m_module->onSignal(tctSig);
}

void ClientSignalHandler::onErrorLocalListener(const ListenErrorMessage& msg)
{
	LOG_INFO("Error in local listen thread(tunnel connection): from %s to %s, type %d\n", m_module->m_ownSessionId.c_str(), msg.m_id.c_str(), TUNNEL_LOCAL_TCP);
	CSLocker lock(&m_module->m_cs);
	std::map<std::string, TunnelConnect*>::iterator it = m_module->m_tunnels.find(msg.m_id);
	if(it != m_module->m_tunnels.end())
	{
		it->second->m_localListenThread->Stop();
		ThreadManager::GetInstance().AddThread(it->second->m_localListenThread);
		it->second->m_localListenThread = 0;
	}
}

void ClientSignalHandler::onErrorLocalTCPConnect(const ConnectErrorMessage& msg)
{
	LOG_INFO("Connection failed(tunnel connection): from %s to %s, type %d\n", m_module->m_ownSessionId.c_str(), msg.m_moduleName.c_str(), TUNNEL_LOCAL_TCP);
}

void ClientSignalHandler::onErrorLocalUDPConnect(const ConnectErrorMessage& msg)
{
	LOG_INFO("Connection failed(tunnel connection): from %s to %s, type %d\n", m_module->m_ownSessionId.c_str(), msg.m_moduleName.c_str(), TUNNEL_LOCAL_UDP);
}

void ClientSignalHandler::onAddLocalTCPConnector(const ConnectorMessage& msg)
{
	TunnelConnector* connector = dynamic_cast<TunnelConnector*>(msg.m_conn);
	if(connector)
	{
		connector->SetTypeConnection(TunnelConnector::LOCAL_TCP);
	}
	m_module->AddConnector(msg.m_conn);
}

void ClientSignalHandler::onAddLocalUDPConnector(const ConnectorMessage& msg)
{
	{
		CSLocker lock(&m_module->m_cs);
		IPCObjectName sessionId(msg.m_conn->GetId());
		std::map<std::string, TunnelConnect*>::iterator it = m_module->m_tunnels.find(sessionId.module_name());
		if(it != m_module->m_tunnels.end())
		{
			it->second->m_localUdpSocket = 0;
		}
	}

	TunnelConnector* connector = dynamic_cast<TunnelConnector*>(msg.m_conn);
	if(connector)
	{
		connector->SetTypeConnection(TunnelConnector::LOCAL_UDP);

	}
	m_module->AddConnector(msg.m_conn);
}

void ClientSignalHandler::onErrorExternalConnect(const ConnectErrorMessage& msg)
{
	LOG_INFO("Connection failed(tunnel connection): from %s to %s, type %d\n", m_module->m_ownSessionId.c_str(), msg.m_moduleName.c_str(), TUNNEL_EXTERNAL);
	CSLocker lock(&m_module->m_cs);
	std::map<std::string, TunnelConnect*>::iterator it = m_module->m_tunnels.find(msg.m_moduleName);
	if(it != m_module->m_tunnels.end())
	{
		if(it->second->m_externalConnectThread)
		{
			it->second->m_externalConnectThread->Stop();
			ThreadManager::GetInstance().AddThread(it->second->m_externalConnectThread);
		}
		it->second->m_externalConnectThread = 0;
	}
}

void ClientSignalHandler::onAddExternalConnector(const ConnectorMessage& msg)
{
	TunnelConnector* connector = dynamic_cast<TunnelConnector*>(msg.m_conn);
	if(connector)
	{
		connector->SetTypeConnection(TunnelConnector::EXTERNAL);
	}
	m_module->AddConnector(msg.m_conn);
}

void ClientSignalHandler::onAddRelayTCPConnector(const ConnectorMessage& msg)
{
	TunnelConnector* connector = dynamic_cast<TunnelConnector*>(msg.m_conn);
	if(connector)
	{
		connector->SetTypeConnection(TunnelConnector::RELAY_TCP);
	}

	m_module->AddConnector(msg.m_conn);
}

void ClientSignalHandler::onAddRelayUDPConnector(const ConnectorMessage& msg)
{
	TunnelConnector* connector = dynamic_cast<TunnelConnector*>(msg.m_conn);
	if(connector)
	{
		connector->SetTypeConnection(TunnelConnector::RELAY_UDP);
	}

	m_module->AddConnector(msg.m_conn);
}

void ClientSignalHandler::onErrorRelayTCPConnect(const ConnectErrorMessage& msg)
{
	LOG_INFO("Connection failed(tunnel connection): from %s to %s, type %d\n", m_module->m_ownSessionId.c_str(), msg.m_moduleName.c_str(), TUNNEL_RELAY_TCP);
}

void ClientSignalHandler::onErrorRelayUDPConnect(const ConnectErrorMessage& msg)
{
	LOG_INFO("Connection failed(tunnel connection): from %s to %s, type %d\n", m_module->m_ownSessionId.c_str(), msg.m_moduleName.c_str(), TUNNEL_RELAY_UDP);
}

void ClientSignalHandler::onModuleName(const ModuleNameMessage& msg)
{
	CSLocker lock(&m_module->m_cs);
	IPCObjectName sessionId(msg.ipc_name());
	std::map<std::string, TunnelConnect*>::iterator it = m_module->m_tunnels.find(sessionId.host_name());
	if(it != m_module->m_tunnels.end())
	{
		delete it->second;
		m_module->m_tunnels.erase(it);
	}
}

void ClientSignalHandler::onConnected(const TunnelConnectedMessage& msg)
{
	InitTunnelComplete itcMsg;
	itcMsg.set_own_session_id(m_module->m_ownSessionId);
	itcMsg.set_ext_session_id(msg.m_id);
	InitTunnelCompleteSignal itcSig(itcMsg);
	m_module->onSignal(itcSig);
	m_module->OnTunnelConnected(msg.m_id, msg.m_type);
}
