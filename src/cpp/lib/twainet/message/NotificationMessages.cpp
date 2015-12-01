#include "NotificationMessages.h"
#include "client_server_lib/module/client_server_module.h"
#include "tunnel_lib/module/tunnel_module.h"

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
		if(callbacks.OnServerConnected)
			callbacks.OnServerConnected(m_module, m_sessionId.c_str());
		name = ClientServerModule::m_serverIPCName;
	}
	else
	{
		if(callbacks.OnClientConnected)
			callbacks.OnClientConnected(m_module, m_sessionId.c_str());
		name = ClientServerModule::m_clientIPCName;
	}
	
	Twainet::ModuleName retName = {0};
#ifdef WIN32
	strcpy_s(retName.m_name, MAX_NAME_LENGTH, name.c_str());
	strcpy_s(retName.m_host, MAX_NAME_LENGTH, m_sessionId.c_str());
#else
	strcpy(retName.m_name, name.c_str());
	strcpy(retName.m_host, m_sessionId.c_str());
#endif/*WIN32*/
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
#ifdef WIN32
	strcpy_s(retName.m_name, MAX_NAME_LENGTH, name.module_name().c_str());
	strcpy_s(retName.m_host, MAX_NAME_LENGTH, name.host_name().c_str());
	strcpy_s(retName.m_connId, MAX_NAME_LENGTH, name.conn_id().c_str());
#else
	strcpy(retName.m_name, name.module_name().c_str());
	strcpy(retName.m_host, name.host_name().c_str());
	strcpy(retName.m_connId, name.conn_id().c_str());
#endif/*WIN32*/
	if(callbacks.OnModuleConnected)
		callbacks.OnModuleConnected(m_module, retName);
}

/*******************************************************************************************/
/*                                ModuleDisconnected                                       */
/*******************************************************************************************/
ModuleDisconnected::ModuleDisconnected(Twainet::Module module, const std::string& id)
	: NotificationMessage(m_module, MODULE_DISCONNECTED), m_id(id)
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
		if(callbacks.OnClientDisconnected)
			callbacks.OnClientDisconnected(m_module, idName.host_name().c_str());
	}
	else if(idName.module_name() == ClientServerModule::m_serverIPCName)
	{
		if(callbacks.OnServerDisconnected)
			callbacks.OnServerDisconnected(m_module);
	}
	else if(idName.module_name() == TunnelModule::m_tunnelIPCName)
	{
		if(callbacks.OnTunnelDisconnected)
			callbacks.OnTunnelDisconnected(m_module, idName.host_name().c_str());
	}

	Twainet::ModuleName retName = {0};
#ifdef WIN32
	strcpy_s(retName.m_name, MAX_NAME_LENGTH, idName.module_name().c_str());
	strcpy_s(retName.m_host, MAX_NAME_LENGTH, idName.host_name().c_str());
	strcpy_s(retName.m_connId, MAX_NAME_LENGTH, idName.conn_id().c_str());
#else
	strcpy(retName.m_name, idName.module_name().c_str());
	strcpy(retName.m_host, idName.host_name().c_str());
	strcpy(retName.m_connId, idName.conn_id().c_str());
#endif/*WIN32*/
	if(callbacks.OnModuleDisconnected)
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
	if(callbacks.OnTunnelConnected)
		callbacks.OnTunnelConnected(m_module, m_sessionId.c_str(), (Twainet::TypeConnection)m_typeConnection);
	Twainet::ModuleName retName = {0};
#ifdef WIN32
	strcpy_s(retName.m_name, MAX_NAME_LENGTH, TunnelModule::m_tunnelIPCName.c_str());
	strcpy_s(retName.m_host, MAX_NAME_LENGTH, m_sessionId.c_str());
#else
	strcpy(retName.m_name, TunnelModule::m_tunnelIPCName.c_str());
	strcpy(retName.m_host, m_sessionId.c_str());
#endif/*WIN32*/
	if(callbacks.OnModuleConnected)
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
		if(callbacks.OnClientConnectionFailed)
			callbacks.OnClientConnectionFailed(m_module);
	}
	else
	{
		IPCObjectName idName = IPCObjectName::GetIPCName(m_id);
		Twainet::ModuleName retName = {0};
#ifdef WIN32
		strcpy_s(retName.m_name, MAX_NAME_LENGTH, idName.module_name().c_str());
		strcpy_s(retName.m_host, MAX_NAME_LENGTH, idName.host_name().c_str());
		strcpy_s(retName.m_connId, MAX_NAME_LENGTH, idName.conn_id().c_str());
#else
		strcpy(retName.m_name, idName.module_name().c_str());
		strcpy(retName.m_host, idName.host_name().c_str());
		strcpy(retName.m_connId, idName.conn_id().c_str());
#endif/*WIN32*/
		if(callbacks.OnModuleConnectionFailed)
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
	Twainet::Message msg = {0};
	msg.m_data = m_data.c_str();
	msg.m_dataLen = m_data.size();
	msg.m_typeMessage = m_messageName.c_str();
	IPCObjectName idName = IPCObjectName::GetIPCName(m_path[0]);
#ifdef WIN32
	strcpy_s((char*)msg.m_target.m_name, MAX_NAME_LENGTH, idName.module_name().c_str());
	strcpy_s((char*)msg.m_target.m_host, MAX_NAME_LENGTH, idName.host_name().c_str());
	strcpy_s((char*)msg.m_target.m_connId, MAX_NAME_LENGTH, idName.conn_id().c_str());
#else
	strcpy((char*)msg.m_target.m_name, idName.module_name().c_str());
	strcpy((char*)msg.m_target.m_host, idName.host_name().c_str());
	strcpy((char*)msg.m_target.m_connId, idName.conn_id().c_str());
#endif/*WIN32*/
		
	if(callbacks.OnMessageRecv)
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
	if(m_type == MODULE && callbacks.OnModuleCreationFailed)
		callbacks.OnModuleCreationFailed(m_module);
	if(m_type == SERVER && callbacks.OnServerCreationFailed)
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
	if(callbacks.OnTunnelCreationFailed)
		callbacks.OnTunnelCreationFailed(m_module, m_sessionId.c_str());
}

/*******************************************************************************************/
/*                                        AuthFailed                                       */
/*******************************************************************************************/
AuthFailed::AuthFailed(Twainet::Module module)
	: NotificationMessage(m_module, CLIENT_AUTH_FAILED)
{
}

AuthFailed::~AuthFailed()
{
}

void AuthFailed::HandleMessage(Twainet::TwainetCallback callbacks)
{
	if(callbacks.OnClientAuthFailed)
		callbacks.OnClientAuthFailed(m_module);
}

/*******************************************************************************************/
/*                           InternalConnectionStatusChanged                               */
/*******************************************************************************************/
InternalConnectionStatusChanged::InternalConnectionStatusChanged(Twainet::Module module, const std::string& moduleName,
								ConnectionStatus status, int port)
	: NotificationMessage(module, INTERNAL_CONNECTION_STATUS), m_moduleName(moduleName)
	, m_status(status), m_port(port)
{
}

InternalConnectionStatusChanged::~InternalConnectionStatusChanged()
{
}

void InternalConnectionStatusChanged::HandleMessage(Twainet::TwainetCallback callbacks)
{
	if(callbacks.OnInternalConnectionStatusChanged)
		callbacks.OnInternalConnectionStatusChanged(m_module, m_moduleName.c_str(),
							   (Twainet::InternalConnectionStatus)m_status, m_port);
	IPCObjectName idName = IPCObjectName::GetIPCName(m_moduleName);
	Twainet::ModuleName retName = {0};
#ifdef WIN32
	strcpy_s(retName.m_name, MAX_NAME_LENGTH, idName.module_name().c_str());
	strcpy_s(retName.m_host, MAX_NAME_LENGTH, idName.host_name().c_str());
	strcpy_s(retName.m_connId, MAX_NAME_LENGTH, idName.conn_id().c_str());
#else
	strcpy(retName.m_name, idName.module_name().c_str());
	strcpy(retName.m_host, idName.host_name().c_str());
	strcpy(retName.m_connId, idName.conn_id().c_str());
#endif/*WIN32*/
	if((Twainet::InternalConnectionStatus)m_status == Twainet::OPEN && callbacks.OnModuleConnected)
	{    
		callbacks.OnModuleConnected(m_module, retName);
	}
	else if((Twainet::InternalConnectionStatus)m_status == Twainet::CLOSE && callbacks.OnModuleDisconnected)
	{
		callbacks.OnModuleDisconnected(m_module, retName);
	}
}