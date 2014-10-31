#ifndef CONNECTOR_MESSAGES_H
#define CONNECTOR_MESSAGES_H

#include "../handler/data_message.h"

/******************************************************************/

class Connector;

class ConnectorMessage : public DataMessage
{
public:
	ConnectorMessage(Connector* conn);
	static std::string GetMessageName();
public:
	Connector* m_conn;
};

/******************************************************************/

class CreatedListenerMessage : public DataMessage
{
public:
	CreatedListenerMessage(const std::string& ip, int port, const std::string& id);
	static std::string GetMessageName();
public:
	std::string m_ip;
	int m_port;
	std::string m_id;
};

/******************************************************************/

class ListenerParamMessage : public DataMessage
{
public:
	ListenerParamMessage(const std::string& moduleName, const std::string& ip = "127.0.0.1", int port = 0);
	static std::string GetMessageName();
public:
	std::string m_moduleName;
	std::string m_ip;
	int m_port;
};

/******************************************************************/

class DisconnectedMessage : public DataMessage
{
public:
	DisconnectedMessage(const std::string& id, const std::string& connId);
	static std::string GetMessageName();
public:
	std::string m_id;
	std::string m_connId;
};

/******************************************************************/

class ConnectedMessage : public DataMessage
{
public:
	ConnectedMessage(const std::string& id);
	static std::string GetMessageName();
public:
	std::string m_id;
};

/******************************************************************/

class ErrorMessage : public DataMessage
{
public:
	ErrorMessage(const std::string& error, int errorCode);
	static std::string GetMessageName();
public:
	std::string m_error;
	int m_errorCode;
};

/******************************************************************/

class ConnectErrorMessage : public ErrorMessage
{
public:
	ConnectErrorMessage(const std::string& moduleName, const std::string& error, int errorCode);
	static std::string GetMessageName();
public:
	std::string m_moduleName;
};

/******************************************************************/

class ListenErrorMessage : public ErrorMessage
{
public:
	ListenErrorMessage(const std::string& id, const std::string& error, int errorCode);
	static std::string GetMessageName();
public:
	std::string m_id;
};

/******************************************************************/

#endif/*CONNECTOR_MESSAGES_H*/