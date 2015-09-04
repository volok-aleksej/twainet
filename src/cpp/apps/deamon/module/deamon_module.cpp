#include "deamon_module.h"

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
	std::vector<Twainet::ModuleName> path;
	for(int i = 0; i < message.m_pathLen; i++)
	{
		path.push_back(message.m_path[i]);
	}
	onData(message.m_typeMessage, path, (char*)message.m_data, message.m_dataLen);
}

void DeamonModule::onMessage(const LocalServerAttributes& msg, const std::vector<Twainet::ModuleName>& path)
{
}














