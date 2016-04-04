#include <time.h>
#include <stdio.h>
#include "tunnel_module.h"
#include "../thread/external_connect_thread.h"
#include "thread_lib/thread/thread_manager.h"
#include "ipc_lib/connector/ipc_connector_factory.h"
#include "connector_lib/socket/socket_factories.h"
#include "connector_lib/socket/udp_socket.h"
#include "common/logger.h"

extern std::vector<std::string> GetLocalIps(int ipv);

const std::string TunnelModule::m_tunnelIPCName = "Tunnel";
const std::string TunnelModule::m_tunnelAccessId = TunnelModule::m_tunnelIPCName;

TunnelModule::TunnelModule(const IPCObjectName& ipcName, ConnectorFactory* factory, int ipv)
	: ClientServerModule(ipcName, factory, ipv)
	, m_clientSignalHandler(this), m_serverSignalHandler(this)
{
	m_tunnelChecker = new TunnelCheckerThread(this);
}

TunnelModule::~TunnelModule()
{
	m_tunnelChecker->Stop();
	
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
	PeerData peerData;
	peerData.set_one_session_id(m_ownSessionId);
	peerData.set_two_session_id(extSessionId);
	if(type == TunnelConnector::LOCAL_TCP)		
		peerData.set_type(TUNNEL_LOCAL_TCP);
	if(type == TunnelConnector::LOCAL_UDP)		
		peerData.set_type(TUNNEL_LOCAL_UDP);
	if(type == TunnelConnector::EXTERNAL)
		peerData.set_type(TUNNEL_EXTERNAL);
	if(type == TunnelConnector::RELAY_TCP)
		peerData.set_type(TUNNEL_RELAY_TCP);
	if(type == TunnelConnector::RELAY_UDP)
		peerData.set_type(TUNNEL_RELAY_UDP);
	if (type != TunnelConnector::UNKNOWN)
	{
		PeerDataSignal pdSig(peerData);
		onSignal(pdSig);
	}
	
	InitTunnel itMsg;
	itMsg.set_own_session_id(m_ownSessionId);
	itMsg.set_ext_session_id(extSessionId);
	itMsg.mutable_address()->set_ip(m_ip);
	itMsg.mutable_address()->set_port(0);
	InitTunnelSignal itSig(itMsg);
	onSignal(itSig);
}

void TunnelModule::DestroyTunnel(const std::string& extSessionId)
{
	LOG_INFO("Destroy tunnel: from %s to %s\n", m_ownSessionId.c_str(), extSessionId.c_str());
	m_manager->StopConnection(extSessionId);
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
	case TunnelConnector::RELAY_TCP:
		peerData.set_type(TUNNEL_RELAY_TCP);
		break;
	case TunnelConnector::RELAY_UDP:
		peerData.set_type(TUNNEL_RELAY_UDP);
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
		ipcSubscribe(clientServerConn, &m_clientSignalHandler, SIGNAL_FUNC(&m_clientSignalHandler, ClientSignalHandler, InitTunnelMessage, onInitTunnel)); //for client
		ipcSubscribe(clientServerConn, &m_serverSignalHandler, SIGNAL_FUNC(&m_serverSignalHandler, ServerSignalHandler, InitTunnelSignal, onInitTunnel)); //for server
		ipcSubscribe(clientServerConn, &m_serverSignalHandler, SIGNAL_FUNC(&m_serverSignalHandler, ServerSignalHandler, PeerDataSignal, onPeerData)); //for server
		ipcSubscribe(clientServerConn, &m_clientSignalHandler, SIGNAL_FUNC(&m_clientSignalHandler, ClientSignalHandler, TryConnectToMessage, onTryConnectTo)); //for client
		ipcSubscribe(clientServerConn, &m_clientSignalHandler, SIGNAL_FUNC(&m_clientSignalHandler, ClientSignalHandler, InitTunnelStartedMessage, onInitTunnelStarted)); //for client
		ipcSubscribe(clientServerConn, &m_serverSignalHandler, SIGNAL_FUNC(&m_serverSignalHandler, ServerSignalHandler, InitTunnelCompleteMessage, onInitTunnelComplete)); //for server
	}

	TunnelConnector* tunnelConnector = dynamic_cast<TunnelConnector*>(connector);
	if(tunnelConnector)
	{
		ipcSubscribe(tunnelConnector, &m_clientSignalHandler, SIGNAL_FUNC(&m_clientSignalHandler, ClientSignalHandler, ModuleNameMessage, onModuleName));
		ipcSubscribe(tunnelConnector, &m_clientSignalHandler, SIGNAL_FUNC(&m_clientSignalHandler, ClientSignalHandler, TunnelConnectedMessage, onConnected));
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

void TunnelModule::FillIPCObjectList(std::vector<IPCObject>& ipcList)
{
	std::vector<IPCObject>::iterator it;
	for(it = ipcList.begin(); it != ipcList.end(); it++)
	{
		if (it->m_ipcName.module_name() == m_tunnelIPCName)
		{
			ipcList.erase(it++);
		}
	}
	
	ClientServerModule::FillIPCObjectList(ipcList);
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
	address.m_socketFactory = new TCPSecureSocketFactory(m_ipv);
	address.m_acceptCount = 1;
	ListenThread* thread = new BaseListenThread(address);
	thread->addSubscriber(&m_clientSignalHandler, SIGNAL_FUNC(&m_clientSignalHandler, ClientSignalHandler, ListenErrorMessage, onErrorLocalListener));
	thread->addSubscriber(&m_clientSignalHandler, SIGNAL_FUNC(&m_clientSignalHandler, ClientSignalHandler, CreatedListenerMessage, onCreatedLocalListener));
	thread->addSubscriber(&m_clientSignalHandler, SIGNAL_FUNC(&m_clientSignalHandler, ClientSignalHandler, ConnectorMessage, onAddLocalTCPConnector));

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
	tunnel->m_localUdpSocket = new UDPSocket((AnySocket::IPVersion)m_ipv);
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
	std::vector<std::string> ips = GetLocalIps(m_ipv);
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
		factory = new TCPSecureProxySocketFactory(m_proxyIp, m_proxyPort, m_proxyUserPassword.m_userName, m_proxyUserPassword.m_password, m_ipv);
	else if(isTCP)
		factory = new TCPSecureSocketFactory(m_ipv);
	else
	{
		UDTSecureSocketFactory *udtFactory = new UDTSecureSocketFactory(m_ipv);
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
	{
		thread->addSubscriber(&m_clientSignalHandler, SIGNAL_FUNC(&m_clientSignalHandler, ClientSignalHandler, ConnectorMessage, onAddLocalTCPConnector));
		thread->addSubscriber(&m_clientSignalHandler, SIGNAL_FUNC(&m_clientSignalHandler, ClientSignalHandler, ConnectErrorMessage, onErrorLocalTCPConnect));
	}
	else
	{
		thread->addSubscriber(&m_clientSignalHandler, SIGNAL_FUNC(&m_clientSignalHandler, ClientSignalHandler, ConnectorMessage, onAddLocalUDPConnector));
		thread->addSubscriber(&m_clientSignalHandler, SIGNAL_FUNC(&m_clientSignalHandler, ClientSignalHandler, ConnectErrorMessage, onErrorLocalUDPConnect));
	}
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
	address.m_socketFactory = new UDPSocketFactory(m_ipv);
	address.m_ip = ip;
	address.m_port = port;
	ExternalConnectThread* thread = new ExternalConnectThread(address);
	thread->addSubscriber(&m_clientSignalHandler, SIGNAL_FUNC(&m_clientSignalHandler, ClientSignalHandler, ConnectorMessage, onAddExternalConnector));
	thread->addSubscriber(&m_clientSignalHandler, SIGNAL_FUNC(&m_clientSignalHandler, ClientSignalHandler, ConnectErrorMessage, onErrorExternalConnect));
	
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
	address.m_socketFactory = new UDTSecureSocketFactory(m_ipv);
	address.m_ip = ip;
	address.m_port = port;
	tunnel->m_externalConnectThread->ChangeConnectAddress(address);
}

void TunnelModule::CreateRelayConnectThread(const std::string& extSessionId, const std::string& ip, int port, bool isTCP)
{
	LOG_INFO("Try creation tunnel: from %s to %s, type %d\n", m_ownSessionId.c_str(), extSessionId.c_str(), TUNNEL_RELAY_TCP);
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
		factory = new TCPSecureProxySocketFactory(m_proxyIp, m_proxyPort, m_proxyUserPassword.m_userName, m_proxyUserPassword.m_password, m_ipv);
	else if(isTCP)
		factory = new TCPSecureSocketFactory(m_ipv);
	else
		factory = new UDTSecureSocketFactory(m_ipv);

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
	{
		thread->addSubscriber(&m_clientSignalHandler, SIGNAL_FUNC(&m_clientSignalHandler, ClientSignalHandler, ConnectorMessage, onAddRelayTCPConnector));
		thread->addSubscriber(&m_clientSignalHandler, SIGNAL_FUNC(&m_clientSignalHandler, ClientSignalHandler, ConnectErrorMessage, onErrorRelayTCPConnect));
	}
	else
	{
		thread->addSubscriber(&m_clientSignalHandler, SIGNAL_FUNC(&m_clientSignalHandler, ClientSignalHandler, ConnectorMessage, onAddRelayUDPConnector));
		thread->addSubscriber(&m_clientSignalHandler, SIGNAL_FUNC(&m_clientSignalHandler, ClientSignalHandler, ConnectErrorMessage, onErrorRelayUDPConnect));
	}
	thread->Start();
}

void TunnelModule::CheckTunnels()
{
	time_t curTime;
	time(&curTime);
	{
		CSLocker lock(&m_cs);
		for(std::map<std::string, TunnelConnect*>::iterator it = m_tunnels.begin();
			it != m_tunnels.end();)
		{
			if(curTime - it->second->m_creationTime > 60)//60 second
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
			if(curTime - it->second->m_creationTime > 60)//60 second
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
	
	std::vector<TunnelStep> tunnelsStep = m_tunnelsStep.GetObjectList();
	for(std::vector<TunnelStep>::iterator it = tunnelsStep.begin();
	    it != tunnelsStep.end(); it++)
	 {
		if(curTime - it->m_creationTime > 60)//60 second
		{
			m_tunnelsStep.RemoveObject(*it);
		}
	 }
}
