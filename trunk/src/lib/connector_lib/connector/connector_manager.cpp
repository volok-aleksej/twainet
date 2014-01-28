#include "connector_manager.h"
#include "message/connector_messages.h"

#include "common/ref.h"
#include "thread_lib/thread/thread_manager.h"


ConnectorManager::ConnectorManager()
{
	
}

ConnectorManager::~ConnectorManager()
{
	OnStop();
}

void ConnectorManager::Stop()
{
	StopThread();
}

void ConnectorManager::OnStart()
{
}

void ConnectorManager::OnStop()
{
	std::map<std::string, std::string> disconnectedModules;
	m_connectors.CheckObjects(Ref(this, &ConnectorManager::StopConnector, disconnectedModules));

	for(std::map<std::string, std::string>::iterator it = disconnectedModules.begin();
		it != disconnectedModules.end(); it++)
	{
		DisconnectedMessage msg(it->second, it->first);
		onSignal(msg);
	}
}

void ConnectorManager::ThreadFunc()
{
	while(!IsStop())
	{
		std::map<std::string, std::string> disconnectedModules;
		m_connectors.CheckObjects(Ref(this, &ConnectorManager::CheckConnection, disconnectedModules));

		for(std::map<std::string, std::string>::iterator it = disconnectedModules.begin();
			it != disconnectedModules.end(); it++)
		{
			DisconnectedMessage msg(it->second, it->first);
			onSignal(msg);
		}

		sleep(100);
	}
}

void ConnectorManager::AddConnection(Connector* conn)
{
	m_connectors.ProcessingObjects(Ref(this, &ConnectorManager::SubscribeConnector, conn));
	m_connectors.AddObject(conn);
	conn->Start();
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

void ConnectorManager::SubscribeConnector(const Connector* newConnector, const Connector* connector)
{
	const_cast<Connector*>(newConnector)->onNewConnector(connector);
}

void ConnectorManager::StopConnectionByName(const std::string& moduleName, const Connector* connector)
{
	if(connector->GetId() == moduleName)
	{
		const_cast<Connector*>(connector)->Stop();
	}
}

bool ConnectorManager::CheckConnection(const std::map<std::string, std::string>& discModules, const Connector* connector)
{
	Connector* conn = const_cast<Connector*>(connector);
	if(conn->IsStopped())
	{
		const_cast<std::map<std::string, std::string>&>(discModules).insert(std::make_pair(conn->GetConnectorId(), conn->GetId()));
		ThreadManager::GetInstance().AddThread(conn);
		return true;
	}

	return false;
}

void ConnectorManager::StopConnectors(const Connector* connector)
{
	const_cast<Connector*>(connector)->Stop();
}

bool ConnectorManager::StopConnector(const std::map<std::string, std::string>& discModules, const Connector* connector)
{
	Connector* conn = const_cast<Connector*>(connector);
	conn->Stop();
	const_cast<std::map<std::string, std::string>&>(discModules).insert(std::make_pair(conn->GetConnectorId(), conn->GetId()));
	ThreadManager::GetInstance().AddThread(conn);
	return true;
}
