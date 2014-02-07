#include "tunnel_messages.h"

/************************************************************************/
/*                    CreatedServerListenerMessage                    */
/************************************************************************/
CreatedServerListenerMessage::CreatedServerListenerMessage(const std::string& id, const std::string& sessionId, const std::string& ip, int port)
: m_ip(ip), m_port(port), m_id(id), m_sessionId(sessionId)
{
}

std::string CreatedServerListenerMessage::GetMessageName()
{
	return typeid(CreatedServerListenerMessage).name();
}

/************************************************************************/
/*                    GotExternalAddressMessage                         */
/************************************************************************/
GotExternalAddressMessage::GotExternalAddressMessage(const std::string& id, const std::string& sessionId, const std::string& ip, int port)
: CreatedServerListenerMessage(id, sessionId, ip, port)
{
}

std::string GotExternalAddressMessage::GetMessageName()
{
	return typeid(GotExternalAddressMessage).name();
}