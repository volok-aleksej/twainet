#ifndef TUNNEL_MESSAGES_H
#define TUNNEL_MESSAGES_H

#include "connector_lib/handler/data_message.h"
#include "connector_lib/message/connector_messages.h"
#include "tunnel_lib/connector/tunnel_connector.h"

class CreatedServerListenerMessage : public DataMessage
{
public:
	CreatedServerListenerMessage(const std::string& id, const std::string& sessionId, const std::string& ip, int port);
	static std::string GetMessageName();
public:
	std::string m_ip;
	int m_port;
	std::string m_id;
	std::string m_sessionId;
};

class GotExternalAddressMessage : public CreatedServerListenerMessage
{
public:
	GotExternalAddressMessage(const std::string& id, const std::string& sessionId, const std::string& ip, int port);
	static std::string GetMessageName();
};

class TunnelConnectedMessage : public ConnectedMessage
{
public:
	TunnelConnectedMessage(const std::string& id, TunnelConnector::TypeConnection type);
	static std::string GetMessageName();
public:
	TunnelConnector::TypeConnection m_type;
};

#endif/*TUNNEL_MESSAGES_H*/