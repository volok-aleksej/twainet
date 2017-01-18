#ifndef CONNECTOR_MANAGER_H
#define CONNECTOR_MANAGER_H

#include <WString.h>

#include "std/list.hpp"
#include "signal.h"
#include "signal_owner.h"

class Connector;

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