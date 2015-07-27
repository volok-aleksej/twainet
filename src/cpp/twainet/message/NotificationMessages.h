#ifndef NOTIFICATION_MESSAGES_H
#define NOTIFICATION_MESSAGES_H

#include "tunnel_lib/connector/tunnel_connector.h"
#include "twainet/application/twainet.h"

enum NotificationType
{
	SERVER_CONNECTED,
	CLIENT_CONNECTED,
	CLIENT_AUTH_FAILED,
	TUNNEL_CONNECTED,
	TUNNEL_DISCONNECT,
	MODULE_CONNECTED,
	MODULE_CONNECT_FAILED,
	MODULE_DISCONNECTED,
	GET_MESSAGE,
	MODULE_CREATION_FAILED,
	SERVER_CREATION_FAILED,
	TUNNEL_CREATION_FAILED
};

class NotificationMessage
{
public:
	NotificationMessage(Twainet::Module module, NotificationType type);
	virtual ~NotificationMessage();

	virtual void HandleMessage(Twainet::TwainetCallback callbacks) = 0;

public:
	Twainet::Module m_module;
	NotificationType m_type;
};

class ClientServerConnected : public NotificationMessage
{
public:
	ClientServerConnected(Twainet::Module module, const std::string& sessionId, bool bClient);
	virtual ~ClientServerConnected();

	virtual void HandleMessage(Twainet::TwainetCallback callbacks);
public:
	std::string m_sessionId;
};

class ModuleConnected : public NotificationMessage
{
public:
	ModuleConnected(Twainet::Module module, const std::string& moduleName);
	virtual ~ModuleConnected();

	virtual void HandleMessage(Twainet::TwainetCallback callbacks);
public:
	std::string m_moduleName;
};

class TunnelConnected : public NotificationMessage
{
public:
	TunnelConnected(Twainet::Module module, const std::string& sessionId, TunnelConnector::TypeConnection type);
	virtual ~TunnelConnected();

	virtual void HandleMessage(Twainet::TwainetCallback callbacks);
public:
	std::string m_sessionId;
	TunnelConnector::TypeConnection m_typeConnection;
};

class ConnectionFailed : public NotificationMessage
{
public:
	ConnectionFailed(Twainet::Module module, const std::string& id);
	virtual ~ConnectionFailed();

	virtual void HandleMessage(Twainet::TwainetCallback callbacks);
public:
	std::string m_id;
};

class ModuleDisconnected : public NotificationMessage
{
public:
	ModuleDisconnected(Twainet::Module module, const std::string& id);
	virtual ~ModuleDisconnected();

	virtual void HandleMessage(Twainet::TwainetCallback callbacks);
public:
	std::string m_id;
};

class GettingMessage : public NotificationMessage
{
public:
	GettingMessage(Twainet::Module module, const std::string& messageName, const std::vector<std::string>& path, const std::string& data);
	virtual ~GettingMessage();

	virtual void HandleMessage(Twainet::TwainetCallback callbacks);
public:
	std::string m_messageName;
	std::vector<std::string> m_path;
	std::string m_data;
};

class CreationFailed : public NotificationMessage
{
public:
	enum CreationType
	{
		MODULE = 0,
		SERVER,
		TUNNEL
	};

	CreationFailed(Twainet::Module module, CreationType type);
	virtual ~CreationFailed();

	virtual void HandleMessage(Twainet::TwainetCallback callbacks);
private:
	CreationType m_type;
};

class TunnelCreationFailed : public CreationFailed
{
public:
	TunnelCreationFailed(Twainet::Module module, const std::string& sessionId);
	virtual ~TunnelCreationFailed();

	virtual void HandleMessage(Twainet::TwainetCallback callbacks);
private:
	std::string m_sessionId;
};

class AuthFailed : public NotificationMessage
{
public:
	AuthFailed(Twainet::Module module);
	virtual ~AuthFailed();

	virtual void HandleMessage(Twainet::TwainetCallback callbacks);
};

#endif/*NOTIFICATION_MESSAGES_H*/