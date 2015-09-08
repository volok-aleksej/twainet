#include "deamon_module.h"
#include "common/common.h"
#include "common/common_func.h"

DeamonModule::DeamonModule(const Twainet::Module& module)
: Module(module)
{
	strcpy(m_userPassword.m_user, CreateGUID().c_str());
	strcpy(m_userPassword.m_pass, CreateGUID().c_str());
	Twainet::SetUsersList(module, &m_userPassword, 1);
	Twainet::CreateServer(module, g_localServerPort);
}

DeamonModule::~DeamonModule()
{
}


void DeamonModule::OnModuleConnected(const Twainet::ModuleName& moduleName)
{
	//TODO: Create read trusted modules from file. Send This Message only if module is trusted
	LocalServerAttributesMessage msg(this);
	msg.set_port(g_localServerPort);
	msg.set_username(m_userPassword.m_user);
	msg.set_password(m_userPassword.m_pass);
	toMessage(msg, &moduleName, 1);
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














