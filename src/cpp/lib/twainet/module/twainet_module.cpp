#include "twainet_module.h"
#include "ipc_lib/connector/ipc_connector_factory.h"
#include "application/application.h"
#include "common/logger.h"

TwainetModule::TwainetModule(const IPCObjectName& ipcName, int ipv)
	: TunnelModule(ipcName, new IPCConnectorFactory<IPCConnector>(ipcName), ipv)
{
}

TwainetModule::~TwainetModule()
{
}

std::vector<IPCObjectName> TwainetModule::GetTargetPath(const IPCObjectName& target)
{
	std::vector<IPCObjectName> retpath;
	std::vector<IPCObject> moduleList = m_modules.GetObjectList();
	for(std::vector<IPCObject>::iterator it = moduleList.begin();
		it != moduleList.end(); it++)
	{
		if (target == it->m_ipcName)
		{
			retpath.push_back(target);
			return retpath;
		}
	}
	
	bool throughtCoordinator = target.host_name().empty();
	std::vector<IPCObject> ipcList = m_ipcObject.GetObjectList();
	for(std::vector<IPCObject>::iterator it = ipcList.begin();
		it != ipcList.end(); it++)
	{
		if(!it->m_ipcName.conn_id().empty())
		{
			continue;
		}
		else if(throughtCoordinator &&	it->m_accessId == IPCModule::m_baseAccessId ||
			!throughtCoordinator &&	it->m_ipcName.module_name() == ClientServerModule::m_serverIPCName)
		{
			retpath.push_back(it->m_ipcName);
			retpath.push_back(target);
		}
	}
		
	return retpath;
}
	
void TwainetModule::OnTunnelConnectFailed(const std::string& sessionId)
{
	LOG_INFO("Tunnel connection failed: sessionId - %s\n", sessionId.c_str());
	Application::GetInstance().AddNotifycationMessage(new TunnelCreationFailed(this, sessionId));
}

void TwainetModule::OnTunnelConnected(const std::string& sessionId, TunnelConnector::TypeConnection type)
{
	LOG_INFO("Tunnel connected: sessionId - %s, type - %d\n", sessionId.c_str(), type);
	Application::GetInstance().AddNotifycationMessage(new TunnelConnected(this, sessionId, type));
}

void TwainetModule::OnServerConnected()
{
	LOG_INFO("Server connected: sessionId - %s\n", m_ownSessionId.c_str());
	Application::GetInstance().AddNotifycationMessage(new ClientServerConnected(this, m_ownSessionId, false));
}

void TwainetModule::OnClientConnector(const std::string& sessionId)
{
	LOG_INFO("Client connected: sessionId - %s\n", sessionId.c_str());
	Application::GetInstance().AddNotifycationMessage(new ClientServerConnected(this, sessionId, true));
}

void TwainetModule::OnFireConnector(const std::string& moduleName)
{
	LOG_INFO("Module disconnected: moduleName - %s\n", moduleName.c_str());
	Application::GetInstance().AddNotifycationMessage(new ModuleDisconnected(this, moduleName));
	TunnelModule::OnFireConnector(moduleName);
}

void TwainetModule::OnConnectFailed(const std::string& moduleName)
{
	LOG_INFO("Module connect failed: moduleName - %s\n", moduleName.c_str());
	Application::GetInstance().AddNotifycationMessage(new ConnectionFailed(this, moduleName));
}

void TwainetModule::OnAuthFailed()
{
	LOG_INFO("Authorization failed\n");
	Application::GetInstance().AddNotifycationMessage(new AuthFailed(this));
}

void TwainetModule::OnConnected(const std::string& moduleName)
{
	LOG_INFO("Module connected: moduleName - %s\n", moduleName.c_str());
	Application::GetInstance().AddNotifycationMessage(new ModuleConnected(this, moduleName));
}

void TwainetModule::OnMessage(const std::string& messageName, const std::vector<std::string>& path, const std::string& data)
{
	Application::GetInstance().AddNotifycationMessage(new GettingMessage(this, messageName, path, data));
}

void TwainetModule::ModuleCreationFialed()
{
	LOG_INFO("Module creation failed: moduleName - %s\n", const_cast<IPCObjectName&>(GetModuleName()).GetModuleNameString().c_str());
	Application::GetInstance().AddNotifycationMessage(new CreationFailed(this, CreationFailed::MODULE));
}

void TwainetModule::ServerCreationFialed()
{
	LOG_INFO("Server creation failed: moduleName - %s\n", const_cast<IPCObjectName&>(GetModuleName()).GetModuleNameString().c_str());
	Application::GetInstance().AddNotifycationMessage(new CreationFailed(this, CreationFailed::SERVER));
}

void TwainetModule::OnInternalConnection(const std::string& moduleName, ConnectionStatus status, int port)
{
	LOG_INFO("internal connection status: moduleName - %s, status - %d, port - %d\n",
		 const_cast<IPCObjectName&>(GetModuleName()).GetModuleNameString().c_str(), status, port);
	Application::GetInstance().AddNotifycationMessage(new InternalConnectionStatusChanged(this, moduleName, status, port));	
}
