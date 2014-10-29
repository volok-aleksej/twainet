#include "twainet_module.h"
#include "ipc_lib\connector\ipc_connector_factory.h"

TwainetModule::TwainetModule(const IPCObjectName& ipcName)
	: TunnelModule(ipcName, new IPCConnectorFactory<IPCConnector>(ipcName))
{
}

TwainetModule::~TwainetModule()
{
}

void TwainetModule::TunnelConnectFailed(const std::string& sessionId)
{
}

void TwainetModule::TunnelConnected(const std::string& sessionId)
{
}

void TwainetModule::OnServerConnected()
{
}

void TwainetModule::OnFireConnector(const std::string& moduleName)
{
}

void TwainetModule::OnConnectFailed(const std::string& moduleName)
{
}