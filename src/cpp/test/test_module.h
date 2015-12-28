#ifndef TEST_MODULE_H
#define TEST_MODULE_H

#include "apps/include/module.h"

class TestModule : public Module
{
public:
	TestModule();
	virtual ~TestModule();
	
public:
	virtual void OnModuleListChanged();
};

#endif/*TEST_MODULE_H*/