#include "test_module.h"
#include "common/common_func.h"
#include <stdio.h>

TestModule::TestModule()
: Module(CreateGUID(), Twainet::IPV4, false)
{
}

TestModule::~TestModule()
{
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