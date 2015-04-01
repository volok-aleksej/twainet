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
	std::string name;
	if(m_type == SERVER_CONNECTED)
	{
		callbacks.OnServerConnected(m_module, m_sessionId.c_str());
		name = ClientServerModule::m_serverIPCName;
	}
	else
	{
		callbacks.OnClientConnected(m_module, m_sessionId.c_str());
		name = ClientServerModule::m_clientIPCName;
	}
	
	Twainet::ModuleName retName = {0};
	strcpy_s(retName.m_name, MAX_NAME_LENGTH, name.c_str());
	strcpy_s(retName.m_host, MAX_NAME_LENGTH, m_sessionId.c_str());
	callbacks.OnModuleConnected(m_module, retName);
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
	IPCObjectName name = IPCObjectName::GetIPCName(m_moduleName);
	Twainet::ModuleName retName = {0};
	strcpy_s(retName.m_name, MAX_NAME_LENGTH, name.module_name().c_str());
	strcpy_s(retName.m_host, MAX_NAME_LENGTH, name.host_name().c_str());
	strcpy_s(retName.m_suffix, MAX_NAME_LENGTH, name.suffix().c_str());
	callbacks.OnModuleConnected(m_module, retName);
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

	Twainet::ModuleName retName = {0};
	strcpy_s(retName.m_name, MAX_NAME_LENGTH, idName.module_name().c_str());
	strcpy_s(retName.m_host, MAX_NAME_LENGTH, idName.host_name().c_str());
	strcpy_s(retName.m_suffix, MAX_NAME_LENGTH, idName.suffix().c_str());
	callbacks.OnModuleDisconnected(m_module, retName);
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
	Twainet::ModuleName retName = {0};
	strcpy_s(retName.m_name, MAX_NAME_LENGTH, m_sessionId.c_str());
	callbacks.OnModuleConnected(m_module, retName);
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
	if(m_id == ClientServerModule::m_serverIPCName)
	{
		callbacks.OnClientConnectionFailed(m_module);
	}
	else
	{
		IPCObjectName idName = IPCObjectName::GetIPCName(m_id);
		Twainet::ModuleName retName = {0};
		strcpy_s(retName.m_name, MAX_NAME_LENGTH, idName.module_name().c_str());
		strcpy_s(retName.m_host, MAX_NAME_LENGTH, idName.host_name().c_str());
		strcpy_s(retName.m_suffix, MAX_NAME_LENGTH, idName.suffix().c_str());
		callbacks.OnModuleConnectionFailed(m_module, retName);
	}
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
	msg.m_pathLen = m_path.size();
	msg.m_path = new Twainet::ModuleName[msg.m_pathLen];
	std::string path;
	for(std::vector<std::string>::iterator it = m_path.begin();
		it != m_path.end(); it++)
	{
		IPCObjectName idName = IPCObjectName::GetIPCName(*it);
		strcpy_s((char*)msg.m_path[it - m_path.begin()].m_name, MAX_NAME_LENGTH, idName.module_name().c_str());
		strcpy_s((char*)msg.m_path[it - m_path.begin()].m_host, MAX_NAME_LENGTH, idName.host_name().c_str());
		strcpy_s((char*)msg.m_path[it - m_path.begin()].m_suffix, MAX_NAME_LENGTH, idName.suffix().c_str());
	}
	callbacks.OnMessageRecv(m_module, msg);
	delete msg.m_path;
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