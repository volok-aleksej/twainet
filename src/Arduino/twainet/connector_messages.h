#ifndef CONNECTOR_MESSAGES_H
#define CONNECTOR_MESSAGES_H

#include <WString.h>

#include "include/data_message.h"

/******************************************************************/

class Connector;

class ConnectorMessage : public DataMessage
{
public:
	ConnectorMessage(Connector* conn);
    virtual char* GetName()const;
	static const char* GetMessageName();
public:
	Connector* m_conn;
};

/******************************************************************/

class DisconnectedMessage : public DataMessage
{
public:
	DisconnectedMessage(const String& id, const String& connId);
    virtual char* GetName()const;
	static const char* GetMessageName();
public:
	String m_id;
	String m_connId;
};

/******************************************************************/

class ConnectedMessage : public DataMessage
{
public:
	ConnectedMessage(const String& id, bool m_bWithCoordinator);
    virtual char* GetName()const;
	static const char* GetMessageName();
public:
	String m_id;
	bool m_bWithCoordinator;
};

/******************************************************************/

class ErrorMessage : public DataMessage
{
public:
	ErrorMessage(const String& error, int errorCode);
    virtual char* GetName()const;
	static const char* GetMessageName();
public:
	String m_error;
	int m_errorCode;
};

/******************************************************************/

#endif/*CONNECTOR_MESSAGES_H*/