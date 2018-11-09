#include "test_module.h"
#include "common/common_func.h"
#include "apps/deamon/module/deamon_module.h"
#include <stdio.h>
#include <netinet/in.h>

TestModule::TestModule()
: Module("twntest2", Twainet::IPV4, false)
{
    AddMessage(new UserMessage<Test, TestModule>(this));
}

TestModule::~TestModule()
{
}

void TestModule::OnModuleConnected(const Twainet::ModuleName& moduleId)
{
    if(strcmp(moduleId.m_name, COORDINATOR_NAME) == 0) {
        Twainet::UserPassword usr_pwd;
        strcpy(usr_pwd.m_user, "test");
        strcpy(usr_pwd.m_pass, "test");
//        Twainet::SetUsersList(m_module, &usr_pwd, 1);
        Twainet::ConnectToServer(m_module, "localhost", 5200, usr_pwd);
//        Twainet::CreateServer(m_module, 5200, Twainet::IPV4, true);
    }
}

void TestModule::OnModuleListChanged()
{
	Module::OnModuleListChanged();

	Twainet::ModuleName* names = 0;
	int sizeNames = 0;
	Twainet::GetExistingModules(GetModule(), names, sizeNames);
	names = new Twainet::ModuleName[sizeNames];
	sizeNames = Twainet::GetExistingModules(GetModule(), names, sizeNames);

	printf("moduleNames changed:\n");
	for(int i = 0; i < sizeNames; i++)
	{
		printf("%d:%s.%s.%s\n", i, names[i].m_name, names[i].m_host, names[i].m_connId);
	}
	printf("\n");

	delete names;
}

void TestModule::OnServerConnected(const char* sessionId)
{
	Module::OnServerConnected(sessionId);

	Twainet::ModuleName moduleName = {0};
	strcpy(moduleName.m_name, "twntest");
	strcpy(moduleName.m_host, sessionId);
	UserMessage<Test, TestModule> cnMsg(this);
	toMessage(cnMsg, moduleName);
}


void TestModule::onMessage(const Test& test, Twainet::ModuleName path)
{
  printf("test message\n");
}
