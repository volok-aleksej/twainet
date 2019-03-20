#include "twainet_module.h"
#include "ipc_lib/connector/ipc_connector_factory.h"
#include "application/application.h"
#include "utils/logger.h"

bool operator == (TwainetModule::WaitData* one,  const TwainetModule::WaitData& two)
{
    return two == *one;
}

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
	if(m_moduleName == target)
	{
		return retpath;
	}
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
	bool throughtServer = !target.host_name().empty();
	std::vector<IPCObject> ipcList = m_ipcObject.GetObjectList();
	for(std::vector<IPCObject>::iterator it = ipcList.begin();
		it != ipcList.end(); it++)
	{
		if(!it->m_ipcName.conn_id().empty())
		{
			continue;
		}
		else if(throughtCoordinator &&	it->m_accessId == IPCModule::m_baseAccessId)
		{
			retpath.push_back(it->m_ipcName);
			retpath.push_back(target);
		}
		else if(throughtServer && it->m_ipcName.module_name() == ClientServerModule::m_serverIPCName)
		{
			retpath.push_back(it->m_ipcName);
			retpath.push_back(IPCObjectName(target.module_name()));
		}
	}
		
	return retpath;
}

bool TwainetModule::SendSyncMsg(const IPCMessageSignal& msg, const std::string& messageName, const IPCObjectName& from, std::string& data)
{
    WaitData waitdata(messageName, from);
    {
        CSLocker lock(&m_cs);
        std::vector<WaitData*>::iterator it = std::find(m_waitingList.begin(), m_waitingList.end(), waitdata);
        if(it != m_waitingList.end()) {
            return false;
        }
        m_waitingList.push_back(&waitdata);
    }
    SendMsg(msg);
    waitdata.m_semaphore.Wait(INFINITE);
    {
        CSLocker lock(&m_cs);
        std::vector<WaitData*>::iterator it = std::find(m_waitingList.begin(), m_waitingList.end(), waitdata);
        if(it != m_waitingList.end()) {
            m_waitingList.erase(it);
        }
    }

    if(!waitdata.m_data.empty()) {
        data.resize(waitdata.m_data.size());
        memcpy((void*)data.c_str(), (void*)waitdata.m_data.c_str(), data.size());
    }
    return true;
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
    CSLocker lock(&m_cs);
    WaitData waitdata(messageName, IPCObjectName::GetIPCName(path[0]));
    std::vector<WaitData*>::iterator it = std::find(m_waitingList.begin(), m_waitingList.end(), waitdata);
    if(it != m_waitingList.end()) {
        if(!data.empty()) {
            (*it)->m_data.resize(data.size());
            memcpy((void*)(*it)->m_data.c_str(), (void*)data.c_str(), data.size());
        }
        (*it)->m_semaphore.Set();
    } else {
        Application::GetInstance().AddNotifycationMessage(new GettingMessage(this, messageName, path, data));
    }
}

void TwainetModule::ModuleCreationFailed()
{
	LOG_INFO("Module creation failed: moduleName - %s\n", const_cast<IPCObjectName&>(GetModuleName()).GetModuleNameString().c_str());
	Application::GetInstance().AddNotifycationMessage(new CreationFailed(this, CreationFailed::MODULE));
}

void TwainetModule::ServerCreationFailed()
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

void TwainetModule::OnIPCObjectsChanged()
{
	LOG_INFO("ipc objects list was changed: moduleName - %s\n",
		 const_cast<IPCObjectName&>(GetModuleName()).GetModuleNameString().c_str());
	Application::GetInstance().AddNotifycationMessage(new ModuleListChanged(this));
}

void TwainetModule::ManagerStop()
{
    {
        CSLocker lock(&m_cs);
        for(std::vector<WaitData*>::iterator it = m_waitingList.begin();
            it != m_waitingList.end(); it++) {
            (*it)->m_semaphore.Set();
        }
    }
    while(true) {
        Thread::sleep(100);
        CSLocker lock(&m_cs);
        if(m_waitingList.empty()) {
            break;
        }
    }
    
    IPCModule::ManagerStop();
}
