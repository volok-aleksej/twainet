#ifndef APP_INTERFACE_H
#define APP_INTERFACE_H

#include "semaphore.h"
#include "singleton.h"
#include "twainet.h"

template<class Application>
class IApplication : public Singleton<Application>
{
public:
	IApplication() : m_bInited(Twainet::IsLibraryInited()), m_bInit(false){}
	virtual ~IApplication(){}

	Twainet::TwainetCallback* Init()
    {
        if(!m_bInit) {
            if(!m_bInited) {
                Twainet::InitLibrary(tc);
            }
            InitializeApplication();
            m_bInit = true;
        }
        return &tc;
    }
    
	int Run()
	{
        Init();
        m_semafor.Wait(INFINITE);
        DeInit();
		return true;
	}
	
	void DeInit()
    {
        if(m_bInit) {
            ShutdownApplication();
            if(!m_bInited) {
                Twainet::CloseLibrary();
            }
            m_bInit = false;
        }
    }
    
	int Stop()
	{
		m_semafor.Set();
		return 0;
	}
	
protected:
	virtual void InitializeApplication() = 0;
    virtual void ShutdownApplication() = 0;
	
public:
	static void TWAINET_CALL OnModuleCreationFailed(Twainet::Module module)
	{
		Application& app = Application::GetInstance();
		app.OnModuleCreationFailed(module);
	}
	
	static void TWAINET_CALL OnServerCreationFailed(Twainet::Module module)
	{
		Application& app = Application::GetInstance();
		app.OnServerCreationFailed(module);
	}
	
	static void TWAINET_CALL OnTunnelCreationFailed(Twainet::Module module, const char* sessionId)
	{
		Application& app = Application::GetInstance();
		app.OnTunnelCreationFailed(module, sessionId);
	}
	
	static void TWAINET_CALL OnServerConnected(Twainet::Module module, const char* sessionId)
	{
		Application& app = Application::GetInstance();
		app.OnServerConnected(module, sessionId);
	}
	
	static void TWAINET_CALL OnClientConnected(Twainet::Module module, const char* sessionId)
	{
		Application& app = Application::GetInstance();
		app.OnClientConnected(module, sessionId);
	}
	
	static void TWAINET_CALL OnClientDisconnected(Twainet::Module module, const char* sessionId)
	{
		Application& app = Application::GetInstance();
		app.OnClientDisconnected(module, sessionId);
	}
	
	static void TWAINET_CALL OnClientConnectionFailed(Twainet::Module module)
	{
		Application& app = Application::GetInstance();
		app.OnClientConnectionFailed(module);
	}
	
	static void TWAINET_CALL OnClientAuthFailed(Twainet::Module module)
	{
		Application& app = Application::GetInstance();
		app.OnClientAuthFailed(module);
	}
	
	static void TWAINET_CALL OnServerDisconnected(Twainet::Module module)
	{
		Application& app = Application::GetInstance();
		app.OnServerDisconnected(module);
	}
	
	static void TWAINET_CALL OnModuleConnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
	{
		Application& app = Application::GetInstance();
		app.OnModuleConnected(module, moduleId);
	}
	
	static void TWAINET_CALL OnModuleDisconnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
	{
		Application& app = Application::GetInstance();
		app.OnModuleDisconnected(module, moduleId);
	}
	
	static void TWAINET_CALL OnModuleConnectionFailed(Twainet::Module module, const Twainet::ModuleName& moduleId)
	{
		Application& app = Application::GetInstance();
		app.OnModuleConnectionFailed(module, moduleId);
	}
	
	static void TWAINET_CALL OnTunnelConnected(Twainet::Module module, const char* sessionId, Twainet::TypeConnection type)
	{
		Application& app = Application::GetInstance();
		app.OnTunnelConnected(module, sessionId, type);
	}
	
	static void TWAINET_CALL OnTunnelDisconnected(Twainet::Module module, const char* sessionId)
	{
		Application& app = Application::GetInstance();
		app.OnTunnelDisconnected(module, sessionId);
	}
	
	static void TWAINET_CALL OnMessageRecv(Twainet::Module module, const Twainet::Message& msg)
	{
		Application& app = Application::GetInstance();
		app.OnMessageRecv(module, msg);
	}
	
	static void TWAINET_CALL OnInternalConnectionStatusChanged(Twainet::Module module, const char* moduleName, Twainet::InternalConnectionStatus status, int port)
	{
		Application& app = Application::GetInstance();
		app.OnInternalConnectionStatusChanged(module, moduleName, status, port);
	}
	
	static void TWAINET_CALL OnModuleListChanged(Twainet::Module module)
	{
		Application& app = Application::GetInstance();
		app.OnModuleListChanged(module);
	}
	
    static Twainet::TwainetCallback tc;
private:
    Semaphore m_semafor;
    bool m_bInited;
    bool m_bInit;
};

template<class Application>
Twainet::TwainetCallback IApplication<Application>::tc = {&IApplication::OnServerConnected, &IApplication::OnServerDisconnected, &IApplication::OnServerCreationFailed,
                                       &IApplication::OnClientConnected, &IApplication::OnClientDisconnected, &IApplication::OnClientConnectionFailed,
                                       &IApplication::OnClientAuthFailed, &IApplication::OnModuleConnected, &IApplication::OnModuleDisconnected,
                                       &IApplication::OnModuleConnectionFailed, &IApplication::OnModuleCreationFailed, &IApplication::OnTunnelConnected,
                                       &IApplication::OnTunnelDisconnected, &IApplication::OnTunnelCreationFailed, &IApplication::OnMessageRecv,
                                       &IApplication::OnInternalConnectionStatusChanged, &IApplication::OnModuleListChanged};
#endif/*APP_INTERFACE_H*/
