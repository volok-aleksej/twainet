#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <vector>
#include "thread_lib/common/critical_section.h"
#include "include/module.h"
#include "include/appinterface.h"

class DeamonApplication : public IApplication<DeamonApplication>
{
	friend class Singleton<DeamonApplication>;
	DeamonApplication();
public:
	virtual ~DeamonApplication();
	
	virtual void InitializeApplication();
	
	static std::string GetAppName();
	static std::string GetDescription();
public:
	virtual void OnModuleCreationFailed(Twainet::Module module);
	virtual void OnServerCreationFailed(Twainet::Module module);
	virtual void OnTunnelCreationFailed(Twainet::Module module, const char* sessionId);
	virtual void OnServerConnected(Twainet::Module module, const char* sessionId);
	virtual void OnClientConnected(Twainet::Module module, const char* sessionId);
	virtual void OnClientDisconnected(Twainet::Module module, const char* sessionId);
	virtual void OnClientConnectionFailed(Twainet::Module module);
	virtual void OnClientAuthFailed(Twainet::Module module);
	virtual void OnServerDisconnected(Twainet::Module module);
	virtual void OnModuleConnected(Twainet::Module module, const Twainet::ModuleName& moduleId);
	virtual void OnModuleDisconnected(Twainet::Module module, const Twainet::ModuleName& moduleId);
	virtual void OnModuleConnectionFailed(Twainet::Module module, const Twainet::ModuleName& moduleId);
	virtual void OnTunnelConnected(Twainet::Module module, const char* sessionId, Twainet::TypeConnection type);
	virtual void OnTunnelDisconnected(Twainet::Module module, const char* sessionId);
	virtual void OnMessageRecv(Twainet::Module module, const Twainet::Message& msg);
	virtual void OnInternalConnectionStatusChanged(Twainet::Module module, const char* moduleName, Twainet::InternalConnectionStatus status, int port);
	virtual void OnModuleListChanged(Twainet::Module module);
private:
	CriticalSection m_cs;
	std::vector<Module*> m_modules;
};

#endif/*APPLICATION_H*/