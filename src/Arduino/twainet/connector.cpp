#include "connector.h"

Connector::Connector(AnySocket* socket)
: Thread(false), m_socket(socket)
{
}

Connector::~Connector()
{
	Stop();
	if(m_socket)
	{
		delete m_socket;
	}
}

void Connector::SetConnectorId(const String& connectorId)
{
	m_connectorId = connectorId;
}

String Connector::GetConnectorId() 
{
	return m_connectorId;
}

void Connector::Stop()
{
	if(m_socket)
	{
		m_socket->Close();
	}
}

bool Connector::SendData(char* data, int len)
{
	if(m_socket)
	{
		return m_socket->Send(data, len);
	}
	return false;
}

void Connector::SetId(const String& id)
{
	if(id.length())
	{
		m_id = id;
	}
}

String Connector::GetId() const
{
	return m_id;
}

void Connector::SetRemoteAddr(const String& ip, int port)
{
	m_remoteIp = ip;
	m_remotePort = port;
}

