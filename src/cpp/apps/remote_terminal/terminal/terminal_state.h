#ifndef TERMINAL_STATE_H
#define TERMINAL_STATE_H

#include <string>
#include "include/object_manager.h"

class Command;

class TerminalState
{
    struct CommandChecker
    {
        std::string cmd;
        const std::vector<std::string>& args;
        Command* command;
    };
public:
    TerminalState();
    virtual ~TerminalState();

    void AddCommand(Command* command);

    virtual bool Check(const std::string& command, const std::vector<std::string>& args);
    virtual void Execute(const std::string& command, const std::vector<std::string>& args);
    virtual std::string GetTerminalName() = 0;
    virtual void Exit() = 0;
protected:
	template<typename TClass, typename TFunc, typename TReturn> friend class Reference;
	template<typename TClass, typename TFunc, typename TObject, typename TReturn> friend class ReferenceObject;
    bool DeleteCommand(const Command* command);
    bool CheckCommand(const CommandChecker& cmd, const Command* command);
    bool ExecuteCommand(const CommandChecker& cmd, const Command* command);
private:
    ObjectManager<Command*> m_commands;
};

class UseTerminal : public TerminalState
{
public:
    UseTerminal();
    virtual std::string GetTerminalName(){ return ""; }
    virtual void Exit(){}
};

class TerminalCommands : public TerminalState
{
public:
    TerminalCommands(const std::string& termName, TerminalState* parent);
    virtual ~TerminalCommands();

    virtual std::string GetTerminalName(){ return m_terminalName; }
    virtual void Exit();
private:
    std::string m_terminalName;
    TerminalState* m_parent;
};

#endif/*TERMINAL_STATE_H*/
