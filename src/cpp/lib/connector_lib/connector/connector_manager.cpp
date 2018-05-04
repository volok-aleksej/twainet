#include "connector_manager.h"
#include "message/connector_messages.h"

#include "include/ref.h"
#include "thread_lib/thread/thread_manager.h"


ConnectorManager::ConnectorManager()
{
	ManagersContainer::GetInstance().AddManager(static_cast<IManager*>(this));
}

ConnectorManager::~ConnectorManager()
{
}

void ConnectorManager::ManagerStart()
{
}

void ConnectorManager::ManagerStop()
{
	std::vector<ModuleInfo> disconnectedModules;
	m_connectors.CheckObjects(Ref(this, &ConnectorManager::StopConnector, disconnectedModules));

	for(std::vector<ModuleInfo>::iterator it = disconnectedModules.begin();
		it != disconnectedModules.end(); it++)
	{
		DisconnectedMessage msg(it->m_id, it->m_connid, it->m_accessid);
		onSignal(msg);
	}
}

void ConnectorManager::ManagerFunc()
{
	std::vector<ModuleInfo> disconnectedModules;
	m_connectors.CheckObjects(Ref(this, &ConnectorManager::CheckConnection, disconnectedModules));

	for(std::vector<ModuleInfo>::iterator it = disconnectedModules.begin();
		it != disconnectedModules.end(); it++)
	{
		DisconnectedMessage msg(it->m_id, it->m_connid, it->m_accessid);
		onSignal(msg);
	}
}

void ConnectorManager::AddConnection(Connector* conn)
{
	m_connectors.ProcessingObjects(Ref(this, &ConnectorManager::SubscribeConnector, conn));
	conn->Start();
	m_connectors.AddObject(conn);
}

void ConnectorManager::StopConnection(const std::string& moduleName)
{
	m_connectors.ProcessingObjects(Ref(this, &ConnectorManager::StopConnectionByName, moduleName));
}

void ConnectorManager::StopAllConnection()
{
	std::vector<std::string> disconnectedModules;
	m_connectors.ProcessingObjects(Ref(this, &ConnectorManager::StopConnectors));
}

bool ConnectorManager::SubscribeConnector(const Connector* newConnector, const Connector* connector)
{
	const_cast<Connector*>(newConnector)->onNewConnector(connector);
	return true;
}

bool ConnectorManager::StopConnectionByName(const std::string& moduleName, const Connector* connector)
{
	if(connector->GetId() == moduleName)
	{
		const_cast<Connector*>(connector)->Stop();
	}
	return true;
}

bool ConnectorManager::CheckConnection(const std::vector<ModuleInfo>& discModules, const Connector* connector)
{
	Connector* conn = const_cast<Connector*>(connector);
	if(conn->IsStopped())
	{
		const_cast<std::vector<ModuleInfo>&>(discModules).push_back(ModuleInfo(conn->GetId(), conn->GetConnectorId(), conn->GetAccessId()));
		ThreadManager::GetInstance().AddThread(conn);
		return true;
	}

	return false;
}

bool ConnectorManager::StopConnectors(const Connector* connector)
{
	const_cast<Connector*>(connector)->Stop();
	return true;
}

bool ConnectorManager::StopConnector(const std::vector<ModuleInfo>& discModules, const Connector* connector)
{
	Connector* conn = const_cast<Connector*>(connector);
	conn->Stop();
	const_cast<std::vector<ModuleInfo>&>(discModules).push_back(ModuleInfo(conn->GetId(), conn->GetConnectorId(), conn->GetAccessId()));
	ThreadManager::GetInstance().AddThread(conn);
	return true;
}
