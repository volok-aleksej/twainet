#include "NotificationMessages.h"
#include "client_server_lib\module\client_server_module.h"

/*******************************************************************************************/
/*                                 NotificationMessage                                     */
/*******************************************************************************************/
NotificationMessage::NotificationMessage(Twainet::Module module, NotificationType type)
	: m_type(type), m_module(module)
{
}

NotificationMessage::~NotificationMessage()
{
}

/*******************************************************************************************/
/*                               ClientServerConnected                                     */
/*******************************************************************************************/
ClientServerConnected::ClientServerConnected(Twainet::Module module, const std::string& sessionId, bool bClient)
	: NotificationMessage(module, bClient ? CLIENT_CONNECTED : SERVER_CONNECTED), m_sessionId(sessionId)
{
}

ClientServerConnected::~ClientServerConnected()
{
}

void ClientServerConnected::HandleMessage(Twainet::TwainetCallback callbacks)
{
	if(m_type == SERVER_CONNECTED)
	{
		callbacks.OnServerConnected(m_module, m_sessionId.c_str());
	}
	else
	{
		callbacks.OnClientConnected(m_module, m_sessionId.c_str());
	}
}

/*******************************************************************************************/
/*                                   ModuleConnected                                       */
/*******************************************************************************************/
ModuleConnected::ModuleConnected(Twainet::Module module, const std::string& moduleName)
	: NotificationMessage(module, MODULE_CONNECTED), m_moduleName(moduleName)
{
}

ModuleConnected::~ModuleConnected()
{
}

void ModuleConnected::HandleMessage(Twainet::TwainetCallback callbacks)
{
	callbacks.OnModuleConnected(m_module, m_moduleName.c_str());
}

/*******************************************************************************************/
/*                                ModuleDisconnected                                       */
/*******************************************************************************************/
ModuleDisconnected::ModuleDisconnected(Twainet::Module module, const std::string& id, bool bTunnel/* = false*/)
	: NotificationMessage(m_module, MODULE_DISCONNECTED), m_id(id), m_bTunnel(bTunnel)
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
		callbacks.OnClientDisconnected(m_module, idName.host_name().c_str());
	}
	else if(idName.module_name() == ClientServerModule::m_serverIPCName)
	{
		callbacks.OnServerDisconnected(m_module);
	}
	else if(m_bTunnel)
	{
		callbacks.OnTunnelDisconnected(m_module, m_id.c_str());
	}
	else
	{
		callbacks.OnModuleDisconnected(m_module, m_id.c_str());
	}
}

/*******************************************************************************************/
/*                                   TunnelConnected                                       */
/*******************************************************************************************/
TunnelConnected::TunnelConnected(Twainet::Module module, const std::string& sessionId, TunnelConnector::TypeConnection type)
	: NotificationMessage(module, TUNNEL_CONNECTED), m_sessionId(sessionId), m_typeConnection(type)
{
}

TunnelConnected::~TunnelConnected()
{
}

void TunnelConnected::HandleMessage(Twainet::TwainetCallback callbacks)
{
	callbacks.OnTunnelConnected(m_module, m_sessionId.c_str(), (Twainet::TypeConnection)m_typeConnection);
}

/*******************************************************************************************/
/*                                  ConnectionFailed                                       */
/*******************************************************************************************/
ConnectionFailed::ConnectionFailed(Twainet::Module module, const std::string& id)
	: NotificationMessage(module, MODULE_CONNECT_FAILED), m_id(id)
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
GettingMessage::GettingMessage(Twainet::Module module, const std::string& messageName, const std::vector<std::string>& path, const std::string& data)
	: NotificationMessage(module, GET_MESSAGE), m_messageName(messageName), m_path(path)
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
	callbacks.OnMessageRecv(m_module, msg);
}

/*******************************************************************************************/
/*                                  CreationFailed                                         */
/*******************************************************************************************/
CreationFailed::CreationFailed(Twainet::Module module, CreationFailed::CreationType type)
	: NotificationMessage(module, NotificationType(MODULE_CREATION_FAILED + type))
	, m_type(type)
{
}

CreationFailed::~CreationFailed()
{
}

void CreationFailed::HandleMessage(Twainet::TwainetCallback callbacks)
{
	if(m_type == MODULE)
		callbacks.OnModuleCreationFailed(m_module);
	if(m_type == SERVER)
		callbacks.OnServerCreationFailed(m_module);
}

/*******************************************************************************************/
/*                                TunnelCreationFailed                                     */
/*******************************************************************************************/
TunnelCreationFailed::TunnelCreationFailed(Twainet::Module module, const std::string& sessionId)
	: CreationFailed(module, TUNNEL), m_sessionId(sessionId)
{
}

TunnelCreationFailed::~TunnelCreationFailed()
{
}

void TunnelCreationFailed::HandleMessage(Twainet::TwainetCallback callbacks)
{
	callbacks.OnTunnelCreationFailed(m_module, m_sessionId.c_str());
}