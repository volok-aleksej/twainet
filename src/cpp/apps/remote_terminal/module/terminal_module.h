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
typedef UserMessage<get_next_command_args, TerminalModule> GetNextCommandArgsMessage;
typedef UserMessage<next_command_args, TerminalModule> NextCommandArgsMessage;

struct ModuleNameCmp
{
    bool operator()(const Twainet::ModuleName& module1, const Twainet::ModuleName& module2)
    {
        int ret = strcmp(module1.m_name, module2.m_name);
        if(ret == 0) {
            ret = strcmp(module1.m_host, module2.m_host);
            if(ret == 0) {
                ret = strcmp(module1.m_connId, module2.m_connId);
            }
        }
        return ret < 0;
    }
};

class TerminalModule : public Module
{
public:
	TerminalModule();
	virtual ~TerminalModule();

    void Init();

    std::vector<std::string> getTerminalNames();

    bool toTermMessage(const DataMessage& msg, const std::string& termName);
    bool toTermMessage(const DataMessage& msg, const std::string& termName, DataMessage& respmsg);

protected:
	virtual void OnModuleConnected(const Twainet::ModuleName& moduleName);
	virtual void OnModuleDisconnected(const Twainet::ModuleName & moduleName);

private:
	template<class TMessage, class THandler> friend class UserMessage;
	void onMessage(const term_name& msg, Twainet::ModuleName path);
	void onMessage(const log& msg, Twainet::ModuleName path);

    //stubs
	void onMessage(const next_command_args& msg, Twainet::ModuleName path){}
	void onMessage(const command& msg, Twainet::ModuleName path){}
	void onMessage(const get_next_command_args& msg, Twainet::ModuleName path){}

private:
	Twainet::UserPassword m_userPassword;
    CriticalSection m_cs;
    std::map<Twainet::ModuleName, std::string, ModuleNameCmp> m_terminalMap;
};

#endif/*TERMINAL_MODULE_H*/
