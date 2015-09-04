#include "deamon_module.h"
#include "deamon_message.h"

DeamonModule::DeamonModule(const Twainet::Module& module)
: Module(module)
{
}

DeamonModule::~DeamonModule()
{
}


void DeamonModule::OnModuleConnected(const Twainet::ModuleName& moduleName)
{
}

void DeamonModule::OnMessageRecv(const Twainet::Message& message)
{
	
}
















