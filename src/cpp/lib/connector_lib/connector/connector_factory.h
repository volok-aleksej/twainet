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

template<class TConnector>
class SimpleConnectorFactory : public ConnectorFactory
{
public:
	SimpleConnectorFactory();
	virtual ~SimpleConnectorFactory ();
	Connector* CreateConnector(AnySocket* socket);
	ConnectorFactory* Clone();
};

template<class TConnector>
SimpleConnectorFactory<TConnector>::SimpleConnectorFactory()
{
}

template<class TConnector>
SimpleConnectorFactory<TConnector>::~SimpleConnectorFactory()
{
}

template<class TConnector>
Connector* SimpleConnectorFactory<TConnector>::CreateConnector(AnySocket* socket)
{
	return static_cast<Connector*>(new TConnector(socket));
}

template<class TConnector>
ConnectorFactory* SimpleConnectorFactory<TConnector>::Clone()
{
	return new SimpleConnectorFactory<TConnector>();
}

#endif/*CONNECTOR_FACTORY_H*/