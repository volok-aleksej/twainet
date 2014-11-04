#include "NotificationMessages.h"
#include "client_server_lib\module\client_server_module.h"

/*******************************************************************************************/
/*                                 NotificationMessage                                     */
/*******************************************************************************************/
NotificationMessage::NotificationMessage(NotificationType type)
	: m_type(type)
{
}

NotificationMessage::~NotificationMessage()
{
}

/*******************************************************************************************/
/*                               ClientServerConnected                                     */
/*******************************************************************************************/
ClientServerConnected::ClientServerConnected(const std::string& sessionId, bool bClient)
	: NotificationMessage(bClient ? CLIENT_CONNECTED : SERVER_CONNECTED), m_sessionId(sessionId)
{
}

ClientServerConnected::~ClientServerConnected()
{
}

void ClientServerConnected::HandleMessage(Twainet::TwainetCallback callbacks)
{
	if(m_type == SERVER_CONNECTED)
	{
		callbacks.OnServerConnected(m_sessionId.c_str());
	}
	else
	{
		callbacks.OnClientConnected(m_sessionId.c_str());
	}
}

/*******************************************************************************************/
/*                                   ModuleConnected                                       */
/*******************************************************************************************/
ModuleConnected::ModuleConnected(const std::string& moduleName)
	: NotificationMessage(MODULE_CONNECTED), m_moduleName(moduleName)
{
}

ModuleConnected::~ModuleConnected()
{
}

void ModuleConnected::HandleMessage(Twainet::TwainetCallback callbacks)
{
	callbacks.OnModuleConnected(m_moduleName.c_str());
}

/*******************************************************************************************/
/*                                ModuleDisconnected                                       */
/*******************************************************************************************/
ModuleDisconnected::ModuleDisconnected(const std::string& id)
	: NotificationMessage(MODULE_DISCONNECTED), m_id(id)
{
}

ModuleDisconnected::~ModuleDisconnected()
{
}

void ModuleDisconnected::HandleMessage(Twainet::TwainetCallback callbacks)
{
	IPCObjectName idName = IPCObjectName::GetIPCName(m_id);
	if(idName.module_name() == ClientServerModule::m_clientIPCName)
	{
		callbacks.OnClientDisconnected(idName.host_name().c_str());
	}
	else if(idName.module_name() == ClientServerModule::m_serverIPCName)
	{
	}
	else
	{
		callbacks.OnModuleDisconnected(m_id.c_str());
	}
}

/*******************************************************************************************/
/*                                   TunnelConnected                                       */
/*******************************************************************************************/
TunnelConnected::TunnelConnected(const std::string& sessionId, TunnelConnector::TypeConnection type)
	: NotificationMessage(TUNNEL_CONNECTED), m_sessionId(sessionId), m_typeConnection(type)
{
}

TunnelConnected::~TunnelConnected()
{
}

void TunnelConnected::HandleMessage(Twainet::TwainetCallback callbacks)
{
	callbacks.OnTunnelConnected(m_sessionId.c_str());
}

/*******************************************************************************************/
/*                                  ConnectionFailed                                       */
/*******************************************************************************************/
ConnectionFailed::ConnectionFailed(const std::string& id, bool bTunnel)
	: NotificationMessage(bTunnel ? TUNNEL_FAILED : MODULE_FAILED), m_id(id)
{
}

ConnectionFailed::~ConnectionFailed()
{
}

void ConnectionFailed::HandleMessage(Twainet::TwainetCallback callbacks)
{
}

/*******************************************************************************************/
/*                                      GetMessage                                         */
/*******************************************************************************************/
GettingMessage::GettingMessage(const std::string& messageName, const std::vector<std::string>& path, const std::string& data)
	: NotificationMessage(GET_MESSAGE), m_messageName(messageName), m_path(path)
{
	m_data.resize(data.size());
	memcpy((void*)m_data.c_str(), data.c_str(), data.size());
}

GettingMessage::~GettingMessage()
{
}

void GettingMessage::HandleMessage(Twainet::TwainetCallback callbacks)
{
	Twainet::Message msg;
	msg.m_data = m_data.c_str();
	msg.m_dataLen = m_data.size();
	msg.m_typeMessage = m_messageName.c_str();
	std::string path;
	for(std::vector<std::string>::iterator it = m_path.begin();
		it != m_path.end(); it++)
	{
		if(it != m_path.begin())
		{
			path.append("->");
		}

		path.append(*it);
	}
	msg.m_path = path.c_str();
	callbacks.OnMessageRecv(msg);
}