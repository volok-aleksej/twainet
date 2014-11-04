#include "client_server_module.h"
#include "connector_lib/thread/connect_thread.h"
#include "connector_lib/socket/socket_factories.h"
#include "ipc_lib/connector/ipc_connector_factory.h"
#include "thread_lib/thread/thread_manager.h"

const std::string ClientServerModule::m_serverIPCName = "ServerName";
const std::string ClientServerModule::m_clientIPCName = "ClientName";

ClientServerModule::ClientServerModule(const IPCObjectName& ipcName, ConnectorFactory* factory)
: IPCModule(ipcName, factory)
, m_isStopConnect(true), m_serverThread(0)
{
	//StartAsCoordinator();
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

	ConnectAddress address;
	address.m_localIP = "";
	address.m_localPort = 0;
	address.m_moduleName = address.m_id = m_serverIPCName;
	address.m_connectorFactory = new IPCConnectorFactory<ClientServerConnector>(m_clientIPCName);
	address.m_socketFactory = new TCPSecureSocketFactory;
//	address.m_socketFactory = new TCPSocketFactory;
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
	printf("\nClient connected to server. sessionId: %s", m_ownSessionId.c_str());
}

void ClientServerModule::OnClientConnector(const std::string& sessionId)
{
	printf("\nServer incoming client connection. sessionId: %s", sessionId.c_str());
}

void ClientServerModule::Disconnect()
{
	IPCObjectName ipcName(m_serverIPCName, m_ownSessionId);
	m_manager.StopConnection(ipcName.GetModuleNameString());
	m_isStopConnect = true;
}

void ClientServerModule::SetUserName(const std::string& userName)
{
	m_userName = userName;
}

void ClientServerModule::SetPassword(const std::string& password)
{
	m_password = password;
}

void ClientServerModule::StartServer(int port)
{
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
	if(m_serverThread)
	{
		m_serverThread->Stop();
		ThreadManager::GetInstance().AddThread(m_serverThread);
		m_serverThread = 0;
	}
}

std::string ClientServerModule::GetSessionId()
{
	return m_ownSessionId;
}

void ClientServerModule::onAddConnector(const ConnectorMessage& msg)
{
	ClientServerConnector* conn = static_cast<ClientServerConnector*>(msg.m_conn);
	if(conn)
	{
		conn->SetUserName(m_userName);
		conn->SetPassword(m_password);
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
		//TODO: Inform about error connect
	}
}

void ClientServerModule::onCreatedListener(const CreatedListenerMessage& msg)
{
}

void ClientServerModule::onErrorListener(const ListenErrorMessage& msg)
{
	ThreadManager::GetInstance().AddThread(m_serverThread);
	m_serverThread = 0;

	//TODO: Inform about error listener
}

void ClientServerModule::onLoginResult(const LoginResultMessage& msg)
{
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
