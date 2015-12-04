#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <vector>
#include "thread_lib/common/critical_section.h"
#include "module.h"
#include "appinterface.h"

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
	void OnModuleCreationFailed(Twainet::Module module);
	void OnServerCreationFailed(Twainet::Module module);
	void OnTunnelCreationFailed(Twainet::Module module, const char* sessionId);
	void OnServerConnected(Twainet::Module module, const char* sessionId);
	void OnClientConnected(Twainet::Module module, const char* sessionId);
	void OnClientDisconnected(Twainet::Module module, const char* sessionId);
	void OnClientConnectionFailed(Twainet::Module module);
	void OnClientAuthFailed(Twainet::Module module);
	void OnServerDisconnected(Twainet::Module module);
	void OnModuleConnected(Twainet::Module module, const Twainet::ModuleName& moduleId);
	void OnModuleDisconnected(Twainet::Module module, const Twainet::ModuleName& moduleId);
	void OnModuleConnectionFailed(Twainet::Module module, const Twainet::ModuleName& moduleId);
	void OnTunnelConnected(Twainet::Module module, const char* sessionId, Twainet::TypeConnection type);
	void OnTunnelDisconnected(Twainet::Module module, const char* sessionId);
	void OnMessageRecv(Twainet::Module module, const Twainet::Message& msg);
	void OnInternalConnectionStatusChanged(Twainet::Module module, const char* moduleName, Twainet::InternalConnectionStatus status, int port);
	void OnModuleListChanged(Twainet::Module module);
private:
	CriticalSection m_cs;
	std::vector<Module*> m_modules;
};

#endif/*APPLICATION_H*/