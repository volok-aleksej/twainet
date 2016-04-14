#ifndef TEST_MODULE_H
#define TEST_MODULE_H

#include "include/module.h"

class TestModule : public Module
{
public:
	TestModule();
	virtual ~TestModule();
	
public:
	virtual void OnModuleListChanged();
	virtual void OnServerConnected(const char* sessionId);
private:
	template<class TMessage, class THandler> friend class DeamonMessage;
	void onMessage(const Test& test, Twainet::ModuleName path);
};

#endif/*TEST_MODULE_H*/