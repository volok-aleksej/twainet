#ifndef MODULE_H
#define MODULE_H

#include "twainet/application/twainet.h"

class Module
{
public:
	Module(const Twainet::Module& module)
	  : m_module(module){}
	virtual ~Module()
	{
		Twainet::DeleteModule(m_module);
	}
	
	virtual void OnTunnelCreationFailed(const char* sessionId){}
	virtual void OnServerConnected(const char* sessionId){}
	virtual void OnClientConnected(const char* sessionId){}
	virtual void OnClientDisconnected(const char* sessionId){}
	virtual void OnClientConnectionFailed(){}
	virtual void OnClientAuthFailed(){}
	virtual void OnServerDisconnected(){}
	virtual void OnModuleConnected(const Twainet::ModuleName& moduleId){}
	virtual void OnModuleDisconnected(const Twainet::ModuleName& moduleId){}
	virtual void OnModuleConnectionFailed(const Twainet::ModuleName& moduleId){}
	virtual void OnTunnelConnected(const char* sessionId, Twainet::TypeConnection type){}
	virtual void OnTunnelDisconnected(const char* sessionId){}
	virtual void OnMessageRecv(const Twainet::Message& msg){}
	virtual void OnInternalConnectionStatusChanged(const char* moduleName, const char* id,
						       Twainet::InternalConnectionStatus status, int port){}
private:
	Twainet::Module m_module;
};

#endif/*MODULE_H*/