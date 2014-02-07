#ifndef CONNECTOR_FACTORY_H
#define CONNECTOR_FACTORY_H

class Connector;
class AnySocket;

class ConnectorFactory
{
public:
	ConnectorFactory(){}
	virtual ~ConnectorFactory(){}
public:
	virtual Connector* CreateConnector(AnySocket* socket) = 0;
	virtual ConnectorFactory* Clone() = 0;
};

#endif/*CONNECTOR_FACTORY_H*/