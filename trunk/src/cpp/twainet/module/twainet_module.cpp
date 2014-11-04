#include "twainet_module.h"
#include "ipc_lib\connector\ipc_connector_factory.h"

TwainetModule::TwainetModule(const IPCObjectName& ipcName)
	: TunnelModule(ipcName, new IPCConnectorFactory<IPCConnector>(ipcName))
{
}

TwainetModule::~TwainetModule()
{
}

void TwainetModule::OnTunnelConnectFailed(const std::string& sessionId)
{
}

void TwainetModule::OnTunnelConnected(const std::string& sessionId, TunnelConnector::TypeConnection type)
{
}

void TwainetModule::OnServerConnected()
{
}

void TwainetModule::OnClientConnector(const std::string& sessionId)
{
}

void TwainetModule::OnFireConnector(const std::string& moduleName)
{
}

void TwainetModule::OnConnectFailed(const std::string& moduleName)
{
}

void TwainetModule::OnConnected(const std::string& moduleName)
{

}

void TwainetModule::OnMessage(const std::string& messageName, const std::vector<std::string>& path, const std::string& data)
{
}