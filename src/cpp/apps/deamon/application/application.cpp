#include "application.h"
#include "thread_lib/thread/thread.h"
#include "module/deamon_module.h"
#ifndef WIN32
#	include <string.h>
#endif/*WIN32*/

#define COORDINATOR_NAME	"twndeamon"

Twainet::TwainetCallback tc = {&DeamonApplication::OnServerConnected, &DeamonApplication::OnServerDisconnected, &DeamonApplication::OnServerCreationFailed,
			       &DeamonApplication::OnClientConnected, &DeamonApplication::OnClientDisconnected, &DeamonApplication::OnClientConnectionFailed,
			       &DeamonApplication::OnClientAuthFailed, &DeamonApplication::OnModuleConnected, &DeamonApplication::OnModuleDisconnected,
			       &DeamonApplication::OnModuleConnectionFailed, &DeamonApplication::OnModuleCreationFailed, &DeamonApplication::OnTunnelConnected,
			       &DeamonApplication::OnTunnelDisconnected, &DeamonApplication::OnTunnelCreationFailed, &DeamonApplication::OnMessageRecv,
			       &DeamonApplication::OnInternalConnectionStatusChanged};

void TWAINET_CALL DeamonApplication::OnModuleCreationFailed(Twainet::Module module)
{
	if(strcmp(Twainet::GetModuleName(module).m_name, COORDINATOR_NAME) == 0)
	{
		DeamonApplication::GetInstance().Stop();
	}
}

void TWAINET_CALL DeamonApplication::OnServerCreationFailed(Twainet::Module module)
{
}

void TWAINET_CALL DeamonApplication::OnTunnelCreationFailed(Twainet::Module module, const char* sessionId)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == *it)
		{
			(*it)->OnTunnelCreationFailed(sessionId);
			break;
		}
	}
}

void TWAINET_CALL DeamonApplication::OnServerConnected(Twainet::Module module, const char* sessionId)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == *it)
		{
			(*it)->OnServerConnected(sessionId);
			break;
		}
	}
}

void TWAINET_CALL DeamonApplication::OnClientConnected(Twainet::Module module, const char* sessionId)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == *it)
		{
			(*it)->OnClientConnected(sessionId);
			break;
		}
	}
}

void TWAINET_CALL DeamonApplication::OnClientDisconnected(Twainet::Module module, const char* sessionId)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == *it)
		{
			(*it)->OnClientDisconnected(sessionId);
			break;
		}
	}
}

void TWAINET_CALL DeamonApplication::OnClientConnectionFailed(Twainet::Module module)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == *it)
		{
			(*it)->OnClientConnectionFailed();
			break;
		}
	}
}

void TWAINET_CALL DeamonApplication::OnClientAuthFailed(Twainet::Module module)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == *it)
		{
			(*it)->OnClientAuthFailed();
			break;
		}
	}
}

void TWAINET_CALL DeamonApplication::OnServerDisconnected(Twainet::Module module)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == *it)
		{
			(*it)->OnServerDisconnected();
			break;
		}
	}
}

void TWAINET_CALL DeamonApplication::OnModuleConnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == *it)
		{
			(*it)->OnModuleConnected(moduleId);
			break;
		}
	}
}

void TWAINET_CALL DeamonApplication::OnModuleDisconnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == *it)
		{
			(*it)->OnModuleDisconnected(moduleId);
			break;
		}
	}
}

void TWAINET_CALL DeamonApplication::OnModuleConnectionFailed(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == *it)
		{
			(*it)->OnModuleConnectionFailed(moduleId);
			break;
		}
	}
}

void TWAINET_CALL DeamonApplication::OnTunnelConnected(Twainet::Module module, const char* sessionId, Twainet::TypeConnection type)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == *it)
		{
			(*it)->OnTunnelConnected(sessionId, type);
			break;
		}
	}
}

void TWAINET_CALL DeamonApplication::OnTunnelDisconnected(Twainet::Module module, const char* sessionId)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == *it)
		{
			(*it)->OnTunnelDisconnected(sessionId);
			break;
		}
	}
}

void TWAINET_CALL DeamonApplication::OnMessageRecv(Twainet::Module module, const Twainet::Message& msg)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == *it)
		{
			(*it)->OnMessageRecv(msg);
			break;
		}
	}
}

void TWAINET_CALL DeamonApplication::OnInternalConnectionStatusChanged(Twainet::Module module, const char* moduleName, const char* id, Twainet::InternalConnectionStatus status, int port)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == *it)
		{
			(*it)->OnInternalConnectionStatusChanged(moduleName, id, status, port);
			break;
		}
	}
}

DeamonApplication::DeamonApplication()
: m_isStop(false)
{
}

DeamonApplication::~DeamonApplication()
{
	CSLocker locker(&GetInstance().m_cs);
	for(std::vector<Module*>::iterator it = m_modules.begin();
	    it != m_modules.end(); it++)
	{
		delete *it;
	}
}

int DeamonApplication::Run()
{
	Twainet::InitLibrary(tc);
	Twainet::ModuleName moduleName = {0};
	strcpy(moduleName.m_name, COORDINATOR_NAME);
	{
		CSLocker locker(&GetInstance().m_cs);
		m_modules.push_back(new DeamonModule(Twainet::CreateModule(moduleName, true)));
	}
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