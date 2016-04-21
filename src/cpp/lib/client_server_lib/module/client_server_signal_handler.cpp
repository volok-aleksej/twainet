#include <stdio.h>
#include <time.h>
#include "client_server_signal_handler.h"
#include "client_server_module.h"
#include "thread_lib/thread/thread_manager.h"
#include "utils/logger.h"

ClientServerSignalHandler::ClientServerSignalHandler(ClientServerModule* module)
: m_module(module)
{
}

ClientServerSignalHandler::~ClientServerSignalHandler()
{
	removeReceiver();
}

void ClientServerSignalHandler::onAddClientServerConnector(const ConnectorMessage& msg)
{
	ClientServerConnector* conn = static_cast<ClientServerConnector*>(msg.m_conn);
	if(conn)
	{
		conn->SetUserName(m_module->m_userPassword.m_userName);
		conn->SetPassword(m_module->m_userPassword.m_password);
		m_module->ipcSubscribe(conn, this, SIGNAL_FUNC(this, ClientServerSignalHandler, LoginMessage, onLogin));
		m_module->ipcSubscribe(conn, this, SIGNAL_FUNC(this, ClientServerSignalHandler, LoginResultMessage, onLoginResult));
		m_module->ipcSubscribe(conn, this, SIGNAL_FUNC(this, ClientServerSignalHandler, ClientServerConnectedMessage, onConnected));
	}

	m_module->AddConnector(msg.m_conn);
}

void ClientServerSignalHandler::onErrorConnect(const ConnectErrorMessage& msg)
{
	if(!m_module->m_isStopConnect && !m_module->m_isExit)
	{
		CSLocker locker(&m_module->m_csServerRequest);
		m_module->m_bConnectToServerRequest = true;
		time(&m_module->m_requestServerCreated);
	}
	else
	{
		m_module->OnConnectFailed(msg.m_moduleName);
	}
}

void ClientServerSignalHandler::onCreatedListener(const CreatedListenerMessage& msg)
{
	LOG_INFO("Server created: m_moduleName - %s\n", m_module->m_moduleName.GetModuleNameString().c_str());
}

void ClientServerSignalHandler::onErrorListener(const ListenErrorMessage& msg)
{
	ThreadManager::GetInstance().AddThread(m_module->m_serverThread);
	m_module->m_serverThread = 0;
	m_module->m_isExit = true;
	m_module->ServerCreationFailed();
}

void ClientServerSignalHandler::onLogin(const LoginMessage& msg)
{
	ClientServerModule::UserPassword user(msg.name());
	if(m_module->m_userPasswords.GetObject(user, &user) && user.m_password == msg.password())
		const_cast<LoginMessage&>(msg).set_login_result(LOGIN_SUCCESS);
	else
		const_cast<LoginMessage&>(msg).set_login_result(LOGIN_FAILURE);
}

void ClientServerSignalHandler::onLoginResult(const LoginResultMessage& msg)
{
	if(msg.login_result() == LOGIN_FAILURE)
	{
		m_module->OnAuthFailed();
		m_module->m_isStopConnect = true;
	}
	else if(m_module->m_ownSessionId.empty())
	{
		m_module->m_ownSessionId = msg.own_session_id();
	}
	else
	{
		m_module->m_manager->StopConnection(IPCObjectName(m_module->m_serverIPCName).GetModuleNameString());
	}
}

void ClientServerSignalHandler::onConnected(const ClientServerConnectedMessage& msg)
{
	IPCObjectName idName = IPCObjectName::GetIPCName(msg.m_id);
	if(idName.module_name() == m_module->m_serverIPCName)
	{
		m_module->OnServerConnected();
	}
	else if(idName.module_name() == m_module->m_clientIPCName)
	{
		m_module->OnClientConnector(idName.host_name());
	}
}
