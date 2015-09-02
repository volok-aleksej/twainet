#include "application.h"
#include "thread_lib/thread/thread.h"
#ifndef WIN32
#	include <string.h>
#endif/*WIN32*/

Twainet::TwainetCallback tc = {&DeamonApplication::OnServerConnected, &DeamonApplication::OnServerDisconnected, &DeamonApplication::OnServerCreationFailed,
							   &DeamonApplication::OnClientConnected, &DeamonApplication::OnClientDisconnected, &DeamonApplication::OnClientConnectionFailed,
							   &DeamonApplication::OnClientAuthFailed, &DeamonApplication::OnModuleConnected, &DeamonApplication::OnModuleDisconnected,
							   &DeamonApplication::OnModuleConnectionFailed, &DeamonApplication::OnModuleCreationFailed, &DeamonApplication::OnTunnelConnected,
							   &DeamonApplication::OnTunnelDisconnected, &DeamonApplication::OnTunnelCreationFailed, &DeamonApplication::OnMessageRecv};

void TWAINET_CALL DeamonApplication::OnModuleCreationFailed(Twainet::Module module)
{
}

void TWAINET_CALL DeamonApplication::OnServerCreationFailed(Twainet::Module module)
{
}

void TWAINET_CALL DeamonApplication::OnTunnelCreationFailed(Twainet::Module module, const char* sessionId)
{
}

void TWAINET_CALL DeamonApplication::OnServerConnected(Twainet::Module module, const char* sessionId)
{
}

void TWAINET_CALL DeamonApplication::OnClientConnected(Twainet::Module module, const char* sessionId)
{
}

void TWAINET_CALL DeamonApplication::OnClientDisconnected(Twainet::Module module, const char* sessionId)
{
}

void TWAINET_CALL DeamonApplication::OnClientConnectionFailed(Twainet::Module module)
{
}

void TWAINET_CALL DeamonApplication::OnClientAuthFailed(Twainet::Module module)
{
}

void TWAINET_CALL DeamonApplication::OnServerDisconnected(Twainet::Module module)
{
}

void TWAINET_CALL DeamonApplication::OnModuleConnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
}

void TWAINET_CALL DeamonApplication::OnModuleDisconnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
}

void TWAINET_CALL DeamonApplication::OnModuleConnectionFailed(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
}

void TWAINET_CALL DeamonApplication::OnTunnelConnected(Twainet::Module module, const char* sessionId, Twainet::TypeConnection type)
{
}

void TWAINET_CALL DeamonApplication::OnTunnelDisconnected(Twainet::Module module, const char* sessionId)
{
}

void TWAINET_CALL DeamonApplication::OnMessageRecv(Twainet::Module module, const Twainet::Message& msg)
{
		//ServerApplication& app = GetInstance();
		//CSLocker locker(&GetInstance().m_cs);
		//for(std::vector<ServerModule*>::iterator it = app.m_modules.begin();
		//	it != app.m_modules.end(); it++)
		//{
		//	if(app.CheckModule(module, *it))
		//	{
		//		(*it)->OnMessageRecv(msg);
		//		break;
		//	}
		//}
}

void TWAINET_CALL DeamonApplication::OnInternalConnectionStatusChanged(Twainet::Module module, const char* moduleName, const char* id, Twainet::InternalConnectionStatus status, int port)
{
}

DeamonApplication::DeamonApplication()
: m_isStop(false)
{
}

DeamonApplication::~DeamonApplication()
{
}

int DeamonApplication::Run()
{
	Twainet::InitLibrary(tc);
	Twainet::ModuleName moduleName = {0};
//	strcpy(moduleName.m_name, "RMMP2PServer");
	//{
	//	CSLocker locker(&GetInstance().m_cs);
	//	m_modules.push_back(new ServerModule(Twainet::CreateModule(moduleName, true)));
	//}
	while(!m_isStop)
	{
		Thread::sleep(200);
	}

	Twainet::FreeLibrary();
	return 0;
}

int DeamonApplication::Stop()
{
	m_isStop = true;
	return 0;
}

std::string DeamonApplication::GetAppName()
{
	return "TwainetDeamon";
}

std::string DeamonApplication::GetDescription()
{
	return "Twainet deamon application";
}