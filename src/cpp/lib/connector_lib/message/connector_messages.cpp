#include "connector_messages.h"

/************************************************************************/
/*                          ConnectorMessage                            */
/************************************************************************/
ConnectorMessage::ConnectorMessage(Connector* conn)
: m_conn(conn)
{
}

std::string ConnectorMessage::GetMessageName()
{
	return typeid(ConnectorMessage).name();
}

/************************************************************************/
/*                       CreatedListenerMessage                         */
/************************************************************************/
CreatedListenerMessage::CreatedListenerMessage(const std::string& ip, int port, const std::string& id)
: m_ip(ip), m_port(port), m_id(id)
{
}

std::string CreatedListenerMessage::GetMessageName()
{
	return typeid(CreatedListenerMessage).name();
}

/************************************************************************/
/*                       ListenerParamsMessage                          */
/************************************************************************/
ListenerParamMessage::ListenerParamMessage(const std::string& moduleName, const std::string& ip, int port)
: m_ip(ip), m_port(port), m_moduleName(moduleName)
{
}

std::string ListenerParamMessage::GetMessageName()
{
	return typeid(ListenerParamMessage).name();
}

/************************************************************************/
/*                              ErrorMessage                            */
/************************************************************************/
ErrorMessage::ErrorMessage(const std::string& error, int errorCode)
: m_error(error), m_errorCode(errorCode)
{
}

std::string ErrorMessage::GetMessageName()
{
	return typeid(ErrorMessage).name();
}

/************************************************************************/
/*                       DisconnectedMessage                            */
/************************************************************************/
DisconnectedMessage::DisconnectedMessage(const std::string& id, const std::string& connId)
: m_id(id), m_connId(connId)
{
}

std::string DisconnectedMessage::GetMessageName()
{
	return typeid(DisconnectedMessage).name();
}

/************************************************************************/
/*                         ConnectedMessage                             */
/************************************************************************/
ConnectedMessage::ConnectedMessage(const std::string& id, bool bWithCoordinator)
: m_id(id), m_bWithCoordinator(bWithCoordinator)
{

}

std::string ConnectedMessage::GetMessageName()
{
	return typeid(ConnectedMessage).name();
}

/************************************************************************/
/*                       ConnectErrorMessage                            */
/************************************************************************/
ConnectErrorMessage::ConnectErrorMessage(const std::string& moduleName, const std::string& error, int errorCode)
: ErrorMessage(error, errorCode), m_moduleName(moduleName)
{
}

std::string ConnectErrorMessage::GetMessageName()
{
	return typeid(ConnectErrorMessage).name();
}

/************************************************************************/
/*                       ListenErrorMessage                             */
/************************************************************************/
ListenErrorMessage::ListenErrorMessage(const std::string& id, const std::string& error, int errorCode)
: ErrorMessage(error, errorCode), m_id(id)
{
}

std::string ListenErrorMessage::GetMessageName()
{
	return typeid(ListenErrorMessage).name();
}
