#include "connector.h"

Connector::Connector(AnySocket* socket)
: m_socket(socket)
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

void Connector::SetConnectorId(const std::string& connectorId)
{
	m_connectorId = connectorId;
}

std::string Connector::GetConnectorId()
{
	return m_connectorId;
}

void Connector::SetAccessId(const std::string& accessId)
{
	m_accessId = accessId;
}

std::string Connector::GetAccessId()
{
	return m_accessId;
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

void Connector::SetId(const std::string& id)
{
	if(!id.empty())
	{
		m_id = id;
	}
}

std::string Connector::GetId()const
{
	return m_id;
}

void Connector::SetRemoteAddr(const std::string& ip, int port)
{
	m_remoteIp = ip;
	m_remotePort = port;
}
