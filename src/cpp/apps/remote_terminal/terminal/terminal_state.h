#ifndef TERMINAL_STATE_H
#define TERMINAL_STATE_H

#include <string>
#include <vector>
#include "include/object_manager.h"
#include "auto_complete_helper.h"

class Command;

class TerminalState : public AutoCompleteHelper
{
public:
    TerminalState();
    virtual ~TerminalState();

    void AddCommand(Command* command);

    virtual bool Execute(const std::string& command, const std::vector<std::string>& args);
    virtual std::vector<std::string> GetArgs(const std::vector<std::string>& args);
    virtual std::string GetTerminalName() = 0;
    virtual void Exit() = 0;
private:
    std::vector<Command*> m_commands;
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

    virtual bool Execute(const std::string& command, const std::vector<std::string>& args);
    virtual std::vector<std::string> GetArgs(const std::vector<std::string>& args);

    virtual std::string GetTerminalName(){ return m_terminalName; }
    virtual void Exit();
private:
    std::string m_terminalName;
    TerminalState* m_parent;
};

#endif/*TERMINAL_STATE_H*/
