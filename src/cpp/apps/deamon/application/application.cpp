#include "application.h"
#include "module/deamon_module.h"
#ifndef WIN32
#	include <string.h>
#endif/*WIN32*/

void DeamonApplication::OnModuleCreationFailed(Twainet::Module module)
{
	if(strcmp(Twainet::GetModuleName(module).m_name, COORDINATOR_NAME) == 0)
	{
		DeamonApplication::GetInstance().Stop();
	}
}

void DeamonApplication::OnServerCreationFailed(Twainet::Module module)
{
	if(strcmp(Twainet::GetModuleName(module).m_name, COORDINATOR_NAME) == 0)
	{
		DeamonApplication::GetInstance().Stop();
	}
}

void DeamonApplication::OnTunnelCreationFailed(Twainet::Module module, const char* sessionId)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == (*it)->GetModule())
		{
			(*it)->OnTunnelCreationFailed(sessionId);
			break;
		}
	}
}

void DeamonApplication::OnServerConnected(Twainet::Module module, const char* sessionId)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == (*it)->GetModule())
		{
			(*it)->OnServerConnected(sessionId);
			break;
		}
	}
}

void DeamonApplication::OnClientConnected(Twainet::Module module, const char* sessionId)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == (*it)->GetModule())
		{
			(*it)->OnClientConnected(sessionId);
			break;
		}
	}
}

void DeamonApplication::OnClientDisconnected(Twainet::Module module, const char* sessionId)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == (*it)->GetModule())
		{
			(*it)->OnClientDisconnected(sessionId);
			break;
		}
	}
}

void DeamonApplication::OnClientConnectionFailed(Twainet::Module module)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == (*it)->GetModule())
		{
			(*it)->OnClientConnectionFailed();
			break;
		}
	}
}

void DeamonApplication::OnClientAuthFailed(Twainet::Module module)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == (*it)->GetModule())
		{
			(*it)->OnClientAuthFailed();
			break;
		}
	}
}

void DeamonApplication::OnServerDisconnected(Twainet::Module module)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == (*it)->GetModule())
		{
			(*it)->OnServerDisconnected();
			break;
		}
	}
}

void DeamonApplication::OnModuleConnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == (*it)->GetModule())
		{
			(*it)->OnModuleConnected(moduleId);
			break;
		}
	}
}

void DeamonApplication::OnModuleDisconnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == (*it)->GetModule())
		{
			(*it)->OnModuleDisconnected(moduleId);
			break;
		}
	}
}

void DeamonApplication::OnModuleConnectionFailed(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == (*it)->GetModule())
		{
			(*it)->OnModuleConnectionFailed(moduleId);
			break;
		}
	}
}

void DeamonApplication::OnTunnelConnected(Twainet::Module module, const char* sessionId, Twainet::TypeConnection type)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == (*it)->GetModule())
		{
			(*it)->OnTunnelConnected(sessionId, type);
			break;
		}
	}
}

void DeamonApplication::OnTunnelDisconnected(Twainet::Module module, const char* sessionId)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == (*it)->GetModule())
		{
			(*it)->OnTunnelDisconnected(sessionId);
			break;
		}
	}
}

void DeamonApplication::OnMessageRecv(Twainet::Module module, const Twainet::Message& msg)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == (*it)->GetModule())
		{
			(*it)->OnMessageRecv(msg);
			break;
		}
	}
}

void DeamonApplication::OnInternalConnectionStatusChanged(Twainet::Module module, const char* moduleName, Twainet::InternalConnectionStatus status, int port)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == (*it)->GetModule())
		{
			(*it)->OnInternalConnectionStatusChanged(moduleName, status, port);
			break;
		}
	}
}

void DeamonApplication::OnModuleListChanged(Twainet::Module module)
{
	DeamonApplication& app = GetInstance();
	CSLocker locker(&app.m_cs);
	for(std::vector<Module*>::iterator it = app.m_modules.begin();
		it != app.m_modules.end(); it++)
	{
		if(module == (*it)->GetModule())
		{
			(*it)->OnModuleListChanged();
			break;
		}
	}
}

DeamonApplication::DeamonApplication()
{
}

DeamonApplication::~DeamonApplication()
{
	CSLocker locker(&GetInstance().m_cs);
	for(std::vector<Module*>::iterator it = m_modules.begin();
	    it != m_modules.end(); it++)
	{
		(*it)->Free();
	}
}

void DeamonApplication::InitializeApplication()
{
	CSLocker locker(&GetInstance().m_cs);
	m_modules.push_back(new DeamonModule);
}

std::string DeamonApplication::GetAppName()
{
	return "TwainetDeamon";
}

std::string DeamonApplication::GetDescription()
{
	return "Twainet deamon application";
}