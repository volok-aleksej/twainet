#include "twainet_module.h"
#include "ipc_lib\connector\ipc_connector_factory.h"
#include "application\application.h"

TwainetModule::TwainetModule(const IPCObjectName& ipcName)
	: TunnelModule(ipcName, new IPCConnectorFactory<IPCConnector>(ipcName))
{
}

TwainetModule::~TwainetModule()
{
}

void TwainetModule::OnTunnelConnectFailed(const std::string& sessionId)
{
	Application::GetInstance().AddNotifycationMessage(new ConnectionFailed(sessionId, true));
}

void TwainetModule::OnTunnelConnected(const std::string& sessionId, TunnelConnector::TypeConnection type)
{
	Application::GetInstance().AddNotifycationMessage(new TunnelConnected(sessionId, type));
}

void TwainetModule::OnServerConnected()
{
	Application::GetInstance().AddNotifycationMessage(new ClientServerConnected(m_ownSessionId, false));
}

void TwainetModule::OnClientConnector(const std::string& sessionId)
{
	Application::GetInstance().AddNotifycationMessage(new ClientServerConnected(sessionId, true));
}

void TwainetModule::OnFireConnector(const std::string& moduleName)
{
	Application::GetInstance().AddNotifycationMessage(new ModuleDisconnected(moduleName));
}

void TwainetModule::OnConnectFailed(const std::string& moduleName)
{
	Application::GetInstance().AddNotifycationMessage(new ConnectionFailed(moduleName, false));
}

void TwainetModule::OnConnected(const std::string& moduleName)
{
	Application::GetInstance().AddNotifycationMessage(new ModuleConnected(moduleName));
}

void TwainetModule::OnMessage(const std::string& messageName, const std::vector<std::string>& path, const std::string& data)
{
	Application::GetInstance().AddNotifycationMessage(new GettingMessage(messageName, path, data));
}