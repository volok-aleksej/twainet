#include "application.h"
#include "module/deamon_module.h"
#ifndef WIN32
#	include <string.h>
#endif/*WIN32*/

DeamonApplication::DeamonApplication()
{
}

DeamonApplication::~DeamonApplication()
{
}

void DeamonApplication::OnModuleCreationFailed(Twainet::Module module)
{
	if(strcmp(Twainet::GetModuleName(module).m_name, COORDINATOR_NAME) == 0)
	{
		Stop();
	}
}

void DeamonApplication::OnServerCreationFailed(Twainet::Module module)
{
	if(strcmp(Twainet::GetModuleName(module).m_name, COORDINATOR_NAME) == 0)
	{
		Stop();
	}
}

void DeamonApplication::InitializeApplication()
{
    DeamonModule* module = new DeamonModule;
    AddModule(module);
    module->Init();
}

std::string DeamonApplication::GetAppName()
{
	return "TwainetDeamon";
}

std::string DeamonApplication::GetDescription()
{
	return "Twainet deamon application";
}