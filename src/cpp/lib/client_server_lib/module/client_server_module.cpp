#include <stdio.h>
#include <time.h>
#include "client_server_module.h"
#include "connector_lib/thread/connect_thread.h"
#include "connector_lib/socket/socket_factories.h"
#include "ipc_lib/connector/ipc_connector_factory.h"
#include "thread_lib/thread/thread_manager.h"
#include "common/logger.h"

const std::string ClientServerModule::m_serverIPCName = "Server";
const std::string ClientServerModule::m_clientIPCName = "Client";

ClientServerModule::ClientServerModule(const IPCObjectName& ipcName, ConnectorFactory* factory, int ipv)
: IPCModule(ipcName, factory, ipv)
, m_isStopConnect(true), m_serverThread(0), m_signalHandler(this)
, m_isUseProxy(false), m_bConnectToServerRequest(false)
{
}

ClientServerModule::~ClientServerModule()
{
	m_isStopConnect = true;

	if(m_serverThread)
	{
		m_serverThread->Stop();
		ThreadManager::GetInstance().AddThread(m_serverThread);
		m_serverThread = 0;
	}
}

void ClientServerModule::Connect(const std::string& ip, int port)
{
	if(m_isExit)
	{
		return;
	}
	
	m_isStopConnect = false;
	m_ip = ip;
	m_port = port;

	SocketFactory* factory;
	if(m_isUseProxy)
		factory = new TCPSecureProxySocketFactory(m_proxyIp, m_proxyPort, m_proxyUserPassword.m_userName, m_proxyUserPassword.m_password, m_ipv);
	else
		factory = new TCPSecureSocketFactory(m_ipv);

	ConnectAddress address;
	address.m_localIP = "";
	address.m_localPort = 0;
	address.m_moduleName = address.m_id = m_serverIPCName;
	address.m_connectorFactory = new IPCConnectorFactory<ClientServerConnector>(m_clientIPCName);
	address.m_socketFactory = factory;
	address.m_ip = ip;
	address.m_port = port;
	ConnectThread* thread = new ConnectThread(address);
	thread->addSubscriber(&m_signalHandler, SIGNAL_FUNC(&m_signalHandler, ClientServerSignalHandler, ConnectorMessage, onAddClientServerConnector));
	thread->addSubscriber(&m_signalHandler, SIGNAL_FUNC(&m_signalHandler, ClientServerSignalHandler, ConnectErrorMessage, onErrorConnect));
	thread->Start();
}

void ClientServerModule::OnFireConnector(const std::string& moduleName)
{
	IPCObjectName ipcModuleName = IPCObjectName::GetIPCName(moduleName);
	if ((ipcModuleName.module_name() == m_serverIPCName && ipcModuleName.host_name() == m_ownSessionId)&&
		!m_isStopConnect && !m_isExit)
	{
		m_ownSessionId.clear();
		CSLocker locker(&m_csServerRequest);
		m_bConnectToServerRequest = true;
		time(&m_requestServerCreated);
		return;
	}
	IPCModule::OnFireConnector(moduleName);
}

bool ClientServerModule::CheckFireConnector(const std::string& moduleName)
{
	IPCObjectName ipcModuleName = IPCObjectName::GetIPCName(moduleName);
	return ipcModuleName.module_name() == m_serverIPCName || ipcModuleName.module_name() == m_clientIPCName;
}

void ClientServerModule::OnServerConnected()
{
	LOG_INFO("Client connected to server. sessionId: %s\n", m_ownSessionId.c_str());
}

void ClientServerModule::OnClientConnector(const std::string& sessionId)
{
	LOG_INFO("Server incoming client connection. sessionId: %s\n", sessionId.c_str());
}

void ClientServerModule::ServerCreationFailed()
{
	LOG_INFO("Server creation failed. m_moduleName %s\n", m_moduleName.GetModuleNameString().c_str());
}

void ClientServerModule::OnAuthFailed()
{
	LOG_INFO("Client authorization failed. m_moduleName %s\n", m_moduleName.GetModuleNameString().c_str());
}

void ClientServerModule::FillIPCObjectList(IPCObjectListMessage& msg)
{
	std::vector<IPCObject> list = m_ipcObject.GetObjectList();
	std::vector<IPCObject>::iterator it;
	for(it = list.begin(); it != list.end(); it++)
	{
		if(it->m_ipcName.module_name() != m_clientIPCName && it->m_ipcName.module_name() != m_serverIPCName)
		{
			AddIPCObject* ipcObject = const_cast<IPCObjectListMessage&>(msg).add_ipc_object();
			ipcObject->set_ip(it->m_ip);
			ipcObject->set_port(it->m_port);
			ipcObject->set_access_id(it->m_accessId);
			*ipcObject->mutable_ipc_name() = it->m_ipcName;
		}
	}
}

void ClientServerModule::ManagerFunc()
{
	IPCModule::ManagerFunc();
	CSLocker locker(&m_csServerRequest);
	time_t t;
	time(&t);
	if(m_bConnectToServerRequest && t - m_requestServerCreated >= m_connectTimeout)
	{
		Connect(m_ip, m_port);
		m_bConnectToServerRequest = false;
	}
}
	
void ClientServerModule::Disconnect()
{
	LOG_INFO("Try client disconnect. sessionId %s\n", m_ownSessionId.c_str());
	IPCObjectName ipcName(m_serverIPCName, m_ownSessionId);
	m_manager->StopConnection(ipcName.GetModuleNameString());
	m_isStopConnect = true;
}

void ClientServerModule::SetUserName(const std::string& userName)
{
	m_userPassword.m_userName = userName;
}

void ClientServerModule::SetPassword(const std::string& password)
{
	m_userPassword.m_password = password;
}

void ClientServerModule::AddUser(const std::string& userName, const std::string& password)
{
	LOG_INFO("Add user %s: m_moduleName - %s\n", userName.c_str(), m_moduleName.GetModuleNameString().c_str());
	UserPassword user(userName, password);
	if(!m_userPasswords.AddObject(user))
		m_userPasswords.UpdateObject(user);
}

void ClientServerModule::RemoveUser(const std::string& userName, const std::string& password)
{
	LOG_INFO("Remove user %s: m_moduleName - %s\n", userName.c_str(), m_moduleName.GetModuleNameString().c_str());
	UserPassword user(userName, password);
	m_userPasswords.RemoveObject(user);
}

void ClientServerModule::ClearUsers()
{
	LOG_INFO("Clear users: m_moduleName - %s\n", m_moduleName.GetModuleNameString().c_str());
	m_userPasswords.Clear();
}

void ClientServerModule::StartServer(int port, int ipv, bool local)
{
	LOG_INFO("Try start server on port %d: m_moduleName - %s\n", port, m_moduleName.GetModuleNameString().c_str());
	if(m_serverThread)
	{
		m_serverThread->Stop();
		ThreadManager::GetInstance().AddThread(m_serverThread);
		m_serverThread = 0;
	}

	ListenAddress address;
	address.m_id = m_clientIPCName;
	address.m_localIP = local ? "127.0.0.1" : "";
	address.m_localPort = port;
	address.m_connectorFactory = new IPCConnectorFactory<ClientServerConnector>(m_serverIPCName);
	address.m_socketFactory = new TCPSecureSocketFactory(AF_INET);
//	address.m_socketFactory = new TCPSocketFactory;
	address.m_acceptCount = -1;
	m_serverThread = new BaseListenThread(address);
	m_serverThread->addSubscriber(&m_signalHandler, SIGNAL_FUNC(&m_signalHandler, ClientServerSignalHandler, CreatedListenerMessage, onCreatedListener));
	m_serverThread->addSubscriber(&m_signalHandler, SIGNAL_FUNC(&m_signalHandler, ClientServerSignalHandler, ListenErrorMessage, onErrorListener));
	m_serverThread->addSubscriber(&m_signalHandler, SIGNAL_FUNC(&m_signalHandler, ClientServerSignalHandler, ConnectorMessage, onAddClientServerConnector));
	m_serverThread->Start();
}

bool ClientServerModule::IsStopServer()
{
	return m_serverThread == 0;
}


void ClientServerModule::StopServer()
{
	LOG_INFO("Try stop server: m_moduleName - %s\n", m_moduleName.GetModuleNameString().c_str());
	if(m_serverThread)
	{
		m_serverThread->Stop();
		ThreadManager::GetInstance().AddThread(m_serverThread);
		m_serverThread = 0;
	}
}

const std::string& ClientServerModule::GetSessionId()
{
	return m_ownSessionId;
}


void ClientServerModule::UseStandartConnections()
{
	LOG_INFO("Use Standard Connectivity: m_moduleName - %s\n", m_moduleName.GetModuleNameString().c_str());
	m_isUseProxy = false;
}

void ClientServerModule::UseProxy(const std::string& ip, int port)
{
	LOG_INFO("Use Connectivity by Proxy: ip - %s, port - %d, m_moduleName - %s\n", ip.c_str(), port, m_moduleName.GetModuleNameString().c_str());
	m_isUseProxy = true;
	m_proxyPort = port;
	m_proxyIp = ip;
}

void ClientServerModule::SetProxyUserName(const std::string& userName)
{
	m_proxyUserPassword.m_userName = userName;
}

void ClientServerModule::SetProxyPassword(const std::string& password)
{
	m_proxyUserPassword.m_password = password;
}
