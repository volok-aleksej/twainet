#ifndef CONNECTOR_MANAGER_H
#define CONNECTOR_MANAGER_H

#include <WString.h>

#include "list.hpp"
#include "connector.h"
#include "signal.h"
#include "signal_owner.h"

class ConnectorManager : public SignalOwner
{
public:
	ConnectorManager();
	~ConnectorManager();

	void AddConnection(Connector* conn);
	void StopConnection(const String& moduleName);
	void StopAllConnection();

private:
	twnstd::list<Connector*> m_connectors;
};

#endif/*CONNECTOR_MANAGER_H*/