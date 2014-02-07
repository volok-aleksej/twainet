#ifndef IPC_CONNECTOR_FACTORY_H
#define IPC_CONNECTOR_FACTORY_H

#include "connector_lib/connector/connector_factory.h"
#include "ipc_lib/module/ipc_object_name.h"

#include <string>

template<class TConnector>
class IPCConnectorFactory : public ConnectorFactory
{
public:
	IPCConnectorFactory(const IPCObjectName& moduleName);
	virtual ~IPCConnectorFactory();
	Connector* CreateConnector(AnySocket* socket);
	ConnectorFactory* Clone();
private:
	IPCObjectName m_moduleName;
};

template<class TConnector>
IPCConnectorFactory<TConnector>::IPCConnectorFactory(const IPCObjectName& moduleName)
: m_moduleName(moduleName)
{
}

template<class TConnector>
IPCConnectorFactory<TConnector>::~IPCConnectorFactory()
{
}

template<class TConnector>
Connector* IPCConnectorFactory<TConnector>::CreateConnector(AnySocket* socket)
{
	return static_cast<Connector*>(new TConnector(socket, m_moduleName));
}

template<class TConnector>
ConnectorFactory* IPCConnectorFactory<TConnector>::Clone()
{
	return new IPCConnectorFactory<TConnector>(m_moduleName);
}

#endif/*IPC_CONNECTOR_FACTORY_H*/