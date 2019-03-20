#ifndef TEST_MODULE_H
#define TEST_MODULE_H

#include "include/module.h"

class TestModule;

#include "apps/remote_terminal/messages/terminal.pb.h"
using namespace terminal;

typedef UserMessage<term_name, TestModule> TermNameMessage;

class TestModule : public Module
{
public:
	TestModule();
	virtual ~TestModule();
	
public:
    virtual void OnModuleConnected(const Twainet::ModuleName& moduleId);
	virtual void OnModuleListChanged();
	virtual void OnServerConnected(const char* sessionId);
private:
	template<class TMessage, class THandler> friend class UserMessage;
	void onMessage(const term_name& test, Twainet::ModuleName path){}
};

#endif/*TEST_MODULE_H*/
