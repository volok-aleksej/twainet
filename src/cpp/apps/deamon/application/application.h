#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <vector>
#include "common/singleton.h"
#include "thread_lib/common/critical_section.h"
#include "twainet/application/twainet.h"
#include "module/module.h"

class DeamonApplication : public Singleton<DeamonApplication>
{
	friend class Singleton<DeamonApplication>;
	DeamonApplication();
public:
	~DeamonApplication();

	int Run();
	int Stop();
	
	static std::string GetAppName();
	static std::string GetDescription();
public:
	static void TWAINET_CALL OnModuleCreationFailed(Twainet::Module module);
	static void TWAINET_CALL OnServerCreationFailed(Twainet::Module module);
	static void TWAINET_CALL OnTunnelCreationFailed(Twainet::Module module, const char* sessionId);
	static void TWAINET_CALL OnServerConnected(Twainet::Module module, const char* sessionId);
	static void TWAINET_CALL OnClientConnected(Twainet::Module module, const char* sessionId);
	static void TWAINET_CALL OnClientDisconnected(Twainet::Module module, const char* sessionId);
	static void TWAINET_CALL OnClientConnectionFailed(Twainet::Module module);
	static void TWAINET_CALL OnClientAuthFailed(Twainet::Module module);
	static void TWAINET_CALL OnServerDisconnected(Twainet::Module module);
	static void TWAINET_CALL OnModuleConnected(Twainet::Module module, const Twainet::ModuleName& moduleId);
	static void TWAINET_CALL OnModuleDisconnected(Twainet::Module module, const Twainet::ModuleName& moduleId);
	static void TWAINET_CALL OnModuleConnectionFailed(Twainet::Module module, const Twainet::ModuleName& moduleId);
	static void TWAINET_CALL OnTunnelConnected(Twainet::Module module, const char* sessionId, Twainet::TypeConnection type);
	static void TWAINET_CALL OnTunnelDisconnected(Twainet::Module module, const char* sessionId);
	static void TWAINET_CALL OnMessageRecv(Twainet::Module module, const Twainet::Message& msg);
	static void TWAINET_CALL OnInternalConnectionStatusChanged(Twainet::Module module, const char* moduleName, Twainet::InternalConnectionStatus status, int port);
	static void TWAINET_CALL OnModuleListChanged(Twainet::Module module);
private:
	CriticalSection m_cs;
	std::vector<Module*> m_modules;
	bool m_isStop;
};

#endif/*APPLICATION_H*/