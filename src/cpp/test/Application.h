#ifndef APPLICATION_H
#define APPLICATION_H

#include "include/twainet.h"
#include "include/appinterface.h"
#include "test_module.h"

class ApplicationTest : public IApplication<ApplicationTest>
{
public:
	ApplicationTest();
	~ApplicationTest();
	
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
	void OnInternalConnectionStatusChanged(Twainet::Module module, const char* moduleName,
					       Twainet::InternalConnectionStatus status, int port);
	void OnModuleListChanged(Twainet::Module module);
	
protected:
	virtual void InitializeApplication();
    virtual void ShutdownApplication();
	
private:
	TestModule* m_module;	
};

extern Twainet::TwainetCallback tc;

#endif/*APPLICATION_H*/