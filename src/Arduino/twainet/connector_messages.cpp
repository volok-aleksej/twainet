#include "connector_messages.h"

/************************************************************************/
/*                          ConnectorMessage                            */
/************************************************************************/
ConnectorMessage::ConnectorMessage(Connector* conn)
: m_conn(conn)
{
}

char* ConnectorMessage::GetName() const
{
    return const_cast<char*>(GetMessageName().c_str());
}

String ConnectorMessage::GetMessageName()
{
	return "ConnectorMessage";
}

/************************************************************************/
/*                              ErrorMessage                            */
/************************************************************************/
ErrorMessage::ErrorMessage(const String& error, int errorCode)
: m_error(error), m_errorCode(errorCode)
{
}

char* ErrorMessage::GetName() const
{
    return const_cast<char*>(GetMessageName().c_str());
}

String ErrorMessage::GetMessageName()
{
	return "ErrorMessage";
}

/************************************************************************/
/*                       DisconnectedMessage                            */
/************************************************************************/
DisconnectedMessage::DisconnectedMessage(const String& id, const String& connId)
: m_id(id), m_connId(connId)
{
}

char* DisconnectedMessage::GetName() const
{
    return const_cast<char*>(GetMessageName().c_str());
}

String DisconnectedMessage::GetMessageName()
{
	return "DisconnectedMessage";
}

/************************************************************************/
/*                         ConnectedMessage                             */
/************************************************************************/
ConnectedMessage::ConnectedMessage(const String& id, bool bWithCoordinator)
: m_id(id), m_bWithCoordinator(bWithCoordinator)
{

}

char* ConnectedMessage::GetName() const
{
    return const_cast<char*>(GetMessageName().c_str());
}

String ConnectedMessage::GetMessageName()
{
	return "ConnectedMessage";
}
