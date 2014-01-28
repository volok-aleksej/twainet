#ifndef ADDRESS_H
#define ADDRESS_H

#include <string>

class SocketFactory;
class ConnectorFactory;

class Address
{
public:
	std::string m_localIP;
	unsigned int m_localPort;

	void operator = (const Address& address)
	{
		m_localIP = address.m_localIP;
		m_localPort = address.m_localPort;
	}
};

class ConnectorAddress : public Address
{
public:
	SocketFactory* m_socketFactory;
	ConnectorFactory* m_connectorFactory;
	std::string m_id;

	void operator = (const ConnectorAddress& address)
	{
		Address::operator = (address);
		m_socketFactory = address.m_socketFactory;
		m_connectorFactory = address.m_connectorFactory;
		m_id = address.m_id;
	}
};

class ConnectAddress : public ConnectorAddress
{
public:
	std::string m_ip;
	unsigned int m_port;
	std::string m_moduleName;
	void operator = (const ConnectAddress& address)
	{
		ConnectorAddress::operator = (address);
		m_ip = address.m_ip;
		m_port = address.m_port;
	}
};

class ListenAddress : public ConnectorAddress
{
public:
	unsigned int m_acceptCount;
};

#endif/*ADDRESS_H*/