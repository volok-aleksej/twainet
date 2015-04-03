#ifndef APPLICATION_H
#define APPLICATION_H

#include "twainet\application\twainet.h"
#include "thread_lib\common\thread_singleton.h"

class Application : public ThreadSingleton<Application>
{
public:
	Application();
	~Application();

	virtual void ThreadFunc();
	virtual void OnStop();
	virtual void OnStart();
	virtual void Stop();
public:
	static void _stdcall OnModuleCreationFailed(Twainet::Module module);
	static void _stdcall OnServerCreationFailed(Twainet::Module module);
	static void _stdcall OnTunnelCreationFailed(Twainet::Module module, const char* sessionId);
	static void _stdcall OnServerConnected(Twainet::Module module, const char* sessionId);
	static void _stdcall OnClientConnected(Twainet::Module module, const char* sessionId);
	static void _stdcall OnClientDisconnected(Twainet::Module module, const char* sessionId);
	static void _stdcall OnClientConnectionFailed(Twainet::Module module);
	static void _stdcall OnServerDisconnected(Twainet::Module module);
	static void _stdcall OnModuleConnected(Twainet::Module module, const Twainet::ModuleName& moduleId);
	static void _stdcall OnModuleDisconnected(Twainet::Module module, const Twainet::ModuleName& moduleId);
	static void _stdcall OnModuleConnectionFailed(Twainet::Module module, const Twainet::ModuleName& moduleId);
	static void _stdcall OnTunnelConnected(Twainet::Module module, const char* sessionId, Twainet::TypeConnection type);
	static void _stdcall OnTunnelDisconnected(Twainet::Module module, const char* sessionId);
	static void _stdcall OnMessageRecv(Twainet::Module module, const Twainet::Message& msg);
protected:
	void onModuleCreationFailed(Twainet::Module module);
	void onServerCreationFailed(Twainet::Module module);
	void onTunnelCreationFailed(Twainet::Module module, const char* sessionId);
	void onServerConnected(Twainet::Module module, const char* sessionId);
	void onClientConnected(Twainet::Module module, const char* sessionId);
	void onClientDisconnected(Twainet::Module module, const char* sessionId);
	void onClientConnectionFailed(Twainet::Module module);
	void onServerDisconnected(Twainet::Module module);
	void onModuleConnected(Twainet::Module module, const Twainet::ModuleName& moduleId);
	void onModuleDisconnected(Twainet::Module module, const Twainet::ModuleName& moduleId);
	void onModuleConnectionFailed(Twainet::Module module, const Twainet::ModuleName& moduleId);
	void onTunnelConnected(Twainet::Module module, const char* sessionId, Twainet::TypeConnection type);
	void onTunnelDisconnected(Twainet::Module module, const char* sessionId);
	void onMessageRecv(Twainet::Module module, const Twainet::Message& msg);
private:
	bool m_isStop;
	Twainet::Module m_module;
	Twainet::ModuleName m_otherModuleName;
	Twainet::ModuleName m_moduleName;
};

extern Twainet::TwainetCallback tc;

#endif/*APPLICATION_H*/