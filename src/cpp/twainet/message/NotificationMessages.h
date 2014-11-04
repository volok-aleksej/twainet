#ifndef NOTIFICATION_MESSAGES_H
#define NOTIFICATION_MESSAGES_H

#include "tunnel_lib\connector\tunnel_connector.h"
#include "twainet\application\twainet.h"

enum NotificationType
{
	SERVER_CONNECTED,
	CLIENT_CONNECTED,
	TUNNEL_CONNECTED,
	TUNNEL_FAILED,
	MODULE_CONNECTED,
	MODULE_FAILED,
	MODULE_DISCONNECTED,
	GET_MESSAGE
};

class NotificationMessage
{
public:
	NotificationMessage(NotificationType type);
	virtual ~NotificationMessage();

	virtual void HandleMessage(Twainet::TwainetCallback callbacks) = 0;

public:
	NotificationType m_type;
};

class ClientServerConnected : public NotificationMessage
{
public:
	ClientServerConnected(const std::string& sessionId, bool bClient);
	virtual ~ClientServerConnected();

	virtual void HandleMessage(Twainet::TwainetCallback callbacks);
public:
	std::string m_sessionId;
};

class ModuleConnected : public NotificationMessage
{
public:
	ModuleConnected(const std::string& moduleName);
	virtual ~ModuleConnected();

	virtual void HandleMessage(Twainet::TwainetCallback callbacks);
public:
	std::string m_moduleName;
};

class TunnelConnected : public NotificationMessage
{
public:
	TunnelConnected(const std::string& sessionId, TunnelConnector::TypeConnection type);
	virtual ~TunnelConnected();

	virtual void HandleMessage(Twainet::TwainetCallback callbacks);
public:
	std::string m_sessionId;
	TunnelConnector::TypeConnection m_typeConnection;
};

class ConnectionFailed : public NotificationMessage
{
public:
	ConnectionFailed(const std::string& id, bool bTunnel);
	virtual ~ConnectionFailed();

	virtual void HandleMessage(Twainet::TwainetCallback callbacks);
public:
	std::string m_id;
};

class ModuleDisconnected : public NotificationMessage
{
public:
	ModuleDisconnected(const std::string& id);
	virtual ~ModuleDisconnected();

	virtual void HandleMessage(Twainet::TwainetCallback callbacks);
public:
	std::string m_id;
};

class GettingMessage : public NotificationMessage
{
public:
	GettingMessage(const std::string& messageName, const std::vector<std::string>& path, const std::string& data);
	virtual ~GettingMessage();

	virtual void HandleMessage(Twainet::TwainetCallback callbacks);
public:
	std::string m_messageName;
	std::vector<std::string> m_path;
	std::string m_data;
};

#endif/*NOTIFICATION_MESSAGES_H*/