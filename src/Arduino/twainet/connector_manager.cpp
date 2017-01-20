#include "connector_manager.h"
#include "connector_messages.h"
#include "connector.h"

ConnectorManager::ConnectorManager()
{
}

ConnectorManager::~ConnectorManager()
{
}

void ConnectorManager::AddConnection(Connector* conn)
{
    for(twnstd::list<Connector*>::iterator it = m_connectors.begin();
        it != m_connectors.end(); ++it) {
        (*it)->onNewConnector(conn);
    }
	conn->StartThread();
	m_connectors.insert(m_connectors.begin(), conn);
}

void ConnectorManager::StopConnection(const String& moduleName)
{
    for(twnstd::list<Connector*>::iterator it = m_connectors.begin();
        it != m_connectors.end(); ++it) {
        if((*it)->GetId() == moduleName) {
            (*it)->StopThread();
            DisconnectedMessage msg((*it)->GetId(), (*it)->GetConnectorId());
            onSignal(msg);
            delete *it;
            m_connectors.erase(it);
            break;
        }
    }
}

void ConnectorManager::StopAllConnection()
{
    for(twnstd::list<Connector*>::iterator it = m_connectors.begin();
        it != m_connectors.end();) {
        (*it)->StopThread();
        DisconnectedMessage msg((*it)->GetId(), (*it)->GetConnectorId());
        onSignal(msg);
        delete *it;
        it = m_connectors.erase(it);
    }
}

