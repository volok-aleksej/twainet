#ifndef TERMINAL_MODULE_H
#define TERMINAL_MODULE_H

#include "include/module.h"

#pragma warning(disable:4244 4267)
#include "messages/terminal.pb.h"
using namespace terminal;
#pragma warning(default:4244 4267)

class TerminalModule;

typedef UserMessage<term_name, TerminalModule> TermNameMessage;
typedef UserMessage<log, TerminalModule> LogMessage;
typedef UserMessage<command, TerminalModule> CommandMessage;

class TerminalModule : public Module
{
public:
	TerminalModule();
	virtual ~TerminalModule();

    void Init();

protected:
	virtual void OnModuleConnected(const Twainet::ModuleName& moduleName);

private:
	template<class TMessage, class THandler> friend class UserMessage;
	void onMessage(const term_name& msg, Twainet::ModuleName path);
	void onMessage(const log& msg, Twainet::ModuleName path);

    //stubs
	void onMessage(const command& msg, Twainet::ModuleName path){}

private:
	Twainet::UserPassword m_userPassword;
};

#endif/*TERMINAL_MODULE_H*/
