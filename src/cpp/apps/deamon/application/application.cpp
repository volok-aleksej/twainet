#include "application.h"
#include "thread_lib/thread/thread.h"
#ifndef WIN32
#	include <string.h>
#endif/*WIN32*/

Twainet::TwainetCallback tc = {&ServerApplication::OnServerConnected, &ServerApplication::OnServerDisconnected, &ServerApplication::OnServerCreationFailed,
							   &ServerApplication::OnClientConnected, &ServerApplication::OnClientDisconnected, &ServerApplication::OnClientConnectionFailed,
							   &ServerApplication::OnClientAuthFailed, &ServerApplication::OnModuleConnected, &ServerApplication::OnModuleDisconnected,
							   &ServerApplication::OnModuleConnectionFailed, &ServerApplication::OnModuleCreationFailed, &ServerApplication::OnTunnelConnected,
							   &ServerApplication::OnTunnelDisconnected, &ServerApplication::OnTunnelCreationFailed, &ServerApplication::OnMessageRecv};

void TWAINET_CALL ServerApplication::OnModuleCreationFailed(Twainet::Module module)
{
}

void TWAINET_CALL ServerApplication::OnServerCreationFailed(Twainet::Module module)
{
}

void TWAINET_CALL ServerApplication::OnTunnelCreationFailed(Twainet::Module module, const char* sessionId)
{
}

void TWAINET_CALL ServerApplication::OnServerConnected(Twainet::Module module, const char* sessionId)
{
}

void TWAINET_CALL ServerApplication::OnClientConnected(Twainet::Module module, const char* sessionId)
{
}

void TWAINET_CALL ServerApplication::OnClientDisconnected(Twainet::Module module, const char* sessionId)
{
}

void TWAINET_CALL ServerApplication::OnClientConnectionFailed(Twainet::Module module)
{
}

void TWAINET_CALL ServerApplication::OnClientAuthFailed(Twainet::Module module)
{
}

void TWAINET_CALL ServerApplication::OnServerDisconnected(Twainet::Module module)
{
}

void TWAINET_CALL ServerApplication::OnModuleConnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
}

void TWAINET_CALL ServerApplication::OnModuleDisconnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
}

void TWAINET_CALL ServerApplication::OnModuleConnectionFailed(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
}

void TWAINET_CALL ServerApplication::OnTunnelConnected(Twainet::Module module, const char* sessionId, Twainet::TypeConnection type)
{
}

void TWAINET_CALL ServerApplication::OnTunnelDisconnected(Twainet::Module module, const char* sessionId)
{
}

void TWAINET_CALL ServerApplication::OnMessageRecv(Twainet::Module module, const Twainet::Message& msg)
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

ServerApplication::ServerApplication()
: m_isStop(false)
{
}

ServerApplication::~ServerApplication()
{
}

int ServerApplication::Run()
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

int ServerApplication::Stop()
{
	m_isStop = true;
	return 0;
}

std::string ServerApplication::GetAppName()
{
	return "TwainetDeamon";
}

std::string ServerApplication::GetDescription()
{
	return "";
}