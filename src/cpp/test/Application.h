#ifndef APPLICATION_H
#define APPLICATION_H

#include "twainet/application/twainet.h"
#include "thread_lib/common/thread_singleton.h"

class ApplicationTest : public ThreadSingleton<ApplicationTest>
{
public:
	ApplicationTest();
	~ApplicationTest();

	virtual void ThreadFunc();
	virtual void OnStop();
	virtual void OnStart();
	virtual void Stop();
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
	static void TWAINET_CALL OnInternalConnectionStatusChanged(Twainet::Module module, const char* moduleName,
								   const char* id, Twainet::InternalConnectionStatus status, int port);
protected:
	void onModuleCreationFailed(Twainet::Module module);
	void onServerCreationFailed(Twainet::Module module);
	void onTunnelCreationFailed(Twainet::Module module, const char* sessionId);
	void onServerConnected(Twainet::Module module, const char* sessionId);
	void onClientConnected(Twainet::Module module, const char* sessionId);
	void onClientDisconnected(Twainet::Module module, const char* sessionId);
	void onClientConnectionFailed(Twainet::Module module);
	void onClientAuthFailed(Twainet::Module module);
	void onServerDisconnected(Twainet::Module module);
	void onModuleConnected(Twainet::Module module, const Twainet::ModuleName& moduleId);
	void onModuleDisconnected(Twainet::Module module, const Twainet::ModuleName& moduleId);
	void onModuleConnectionFailed(Twainet::Module module, const Twainet::ModuleName& moduleId);
	void onTunnelConnected(Twainet::Module module, const char* sessionId, Twainet::TypeConnection type);
	void onTunnelDisconnected(Twainet::Module module, const char* sessionId);
	void onMessageRecv(Twainet::Module module, const Twainet::Message& msg);
	void onInternalConnectionStatusChanged(Twainet::Module module, const char* moduleName,
										   const char* id, Twainet::InternalConnectionStatus status, int port);
private:
	bool m_isStop;
	Twainet::Module m_module;
	Twainet::ModuleName m_otherModuleName;
	Twainet::ModuleName m_moduleName;
};

extern Twainet::TwainetCallback tc;

#endif/*APPLICATION_H*/