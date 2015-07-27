#include <stdio.h>
#include "client_server_module.h"
#include "connector_lib/thread/connect_thread.h"
#include "connector_lib/socket/socket_factories.h"
#include "ipc_lib/connector/ipc_connector_factory.h"
#include "thread_lib/thread/thread_manager.h"
#include "common/logger.h"

const std::string ClientServerModule::m_serverIPCName = "Server";
const std::string ClientServerModule::m_clientIPCName = "Client";

ClientServerModule::ClientServerModule(const IPCObjectName& ipcName, ConnectorFactory* factory)
: IPCModule(ipcName, factory)
, m_isStopConnect(true), m_serverThread(0)
, m_isUseProxy(false)
{
}

ClientServerModule::~ClientServerModule()
{
	m_isStopConnect = true;
	removeReceiver();

	if(m_serverThread)
	{
		m_serverThread->Stop();
		ThreadManager::GetInstance().AddThread(m_serverThread);
		m_serverThread = 0;
	}
}

void ClientServerModule::Connect(const std::string& ip, int port)
{
	m_isStopConnect = false;
	m_ip = ip;
	m_port = port;

	SocketFactory* factory;
	if(m_isUseProxy)
		factory = new TCPSecureProxySocketFactory(m_proxyIp, m_proxyPort, m_proxyUserPassword.m_userName, m_proxyUserPassword.m_password);
	else
		factory = new TCPSecureSocketFactory;

	ConnectAddress address;
	address.m_localIP = "";
	address.m_localPort = 0;
	address.m_moduleName = address.m_id = m_serverIPCName;
	address.m_connectorFactory = new IPCConnectorFactory<ClientServerConnector>(m_clientIPCName);
	address.m_socketFactory = factory;
	address.m_ip = ip;
	address.m_port = port;
	ConnectThread* thread = new ConnectThread(address);
	thread->addSubscriber(this, SIGNAL_FUNC(this, ClientServerModule, ConnectorMessage, onAddConnector));
	thread->addSubscriber(this, SIGNAL_FUNC(this, ClientServerModule, ConnectErrorMessage, onErrorConnect));
	thread->Start();
}

void ClientServerModule::OnFireConnector(const std::string& moduleName)
{
	IPCObjectName ipcModuleName = IPCObjectName::GetIPCName(moduleName);
	if(ipcModuleName.module_name() == m_serverIPCName && !m_isStopConnect)
	{
		m_ownSessionId.clear();
		Connect(m_ip, m_port);
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
			*ipcObject->mutable_ipc_name() = it->m_ipcName;
		}
	}
}

void ClientServerModule::Disconnect()
{
	LOG_INFO("Try client disconnect. sessionId %s\n", m_ownSessionId.c_str());
	IPCObjectName ipcName(m_serverIPCName, m_ownSessionId);
	m_manager.StopConnection(ipcName.GetModuleNameString());
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

void ClientServerModule::StartServer(int port)
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
	address.m_localIP = "";
	address.m_localPort = port;
	address.m_connectorFactory = new IPCConnectorFactory<ClientServerConnector>(m_serverIPCName);
	address.m_socketFactory = new TCPSecureSocketFactory;
//	address.m_socketFactory = new TCPSocketFactory;
	address.m_acceptCount = -1;
	m_serverThread = new IPCListenThread(address);
	m_serverThread->addSubscriber(this, SIGNAL_FUNC(this, ClientServerModule, CreatedListenerMessage, onCreatedListener));
	m_serverThread->addSubscriber(this, SIGNAL_FUNC(this, ClientServerModule, ListenErrorMessage, onErrorListener));
	m_serverThread->addSubscriber(this, SIGNAL_FUNC(this, ClientServerModule, ConnectorMessage, onAddConnector));
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

void ClientServerModule::onAddConnector(const ConnectorMessage& msg)
{
	ClientServerConnector* conn = static_cast<ClientServerConnector*>(msg.m_conn);
	if(conn)
	{
		conn->SetUserName(m_userPassword.m_userName);
		conn->SetPassword(m_userPassword.m_password);
		ipcSubscribe(conn, SIGNAL_FUNC(this, ClientServerModule, LoginMessage, onLogin));
		ipcSubscribe(conn, SIGNAL_FUNC(this, ClientServerModule, LoginResultMessage, onLoginResult));
		ipcSubscribe(conn, SIGNAL_FUNC(this, ClientServerModule, ClientServerConnectedMessage, onConnected));
	}

	IPCModule::onAddConnector(msg);
}

void ClientServerModule::onErrorConnect(const ConnectErrorMessage& msg)
{
	if(!m_isStopConnect)
	{
		Connect(m_ip, m_port);
	}
	else
	{
		OnConnectFailed(msg.m_moduleName);
	}
}

void ClientServerModule::onCreatedListener(const CreatedListenerMessage& msg)
{
	LOG_INFO("Server created: m_moduleName - %s\n", m_moduleName.GetModuleNameString().c_str());
}

void ClientServerModule::onErrorListener(const ListenErrorMessage& msg)
{
	ThreadManager::GetInstance().AddThread(m_serverThread);
	m_serverThread = 0;
	m_isExit = true;
	ServerCreationFailed();
}

void ClientServerModule::onLogin(const LoginMessage& msg)
{
	UserPassword user(msg.name());
	if(m_userPasswords.GetObject(user, &user) && user.m_password == msg.password())
		const_cast<LoginMessage&>(msg).set_login_result(LOGIN_SUCCESS);
	else
		const_cast<LoginMessage&>(msg).set_login_result(LOGIN_FAILURE);
}

void ClientServerModule::onLoginResult(const LoginResultMessage& msg)
{
	if(msg.login_result() == LOGIN_FAILURE)
	{
		OnAuthFailed();
		m_isStopConnect = true;
	}
	else
		m_ownSessionId = msg.own_session_id();
}

void ClientServerModule::onConnected(const ClientServerConnectedMessage& msg)
{
	IPCObjectName idName = IPCObjectName::GetIPCName(msg.m_id);
	if(idName.module_name() == m_serverIPCName)
	{
		OnServerConnected();
	}
	else if(idName.module_name() == m_clientIPCName)
	{
		OnClientConnector(idName.host_name());
	}
}
