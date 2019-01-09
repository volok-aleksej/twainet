class TermState;
#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <vector>
#include "terminal_state.h"
#include "auto_complete_helper.h"

class Terminal;

class Command
{
public:
    explicit Command(const std::string& command)
    : m_command(command){}
    virtual ~Command(){}

    virtual void Execute(const std::vector<std::string>& args) = 0;
    virtual bool Check(const std::string& command, const std::vector<std::string>& args) const;
    virtual std::string GetCommand() const { return m_command; }
    virtual std::vector<std::string> GetArgs(const std::vector<std::string>& args) = 0;

    bool operator == (const std::string& command) const {
        return m_command == command;
    }
    bool operator != (const std::string& command) const {
        return m_command != command;
    }
protected:
    std::string m_command;
};

class UseCommand : public Command, public AutoCompleteHelper
{
public:
    UseCommand();
    virtual ~UseCommand();

    virtual void Execute(const std::vector<std::string>& args);
    virtual bool Check(const std::string& command, const std::vector<std::string>& args) const;
    virtual std::vector<std::string> GetArgs(const std::vector<std::string>& args);
};

class TerminalCommand : public Command
{
public:
    TerminalCommand(const std::string& command, const std::vector<std::string>& args, TerminalState* state);
    virtual ~TerminalCommand();

    virtual void Execute(const std::vector<std::string>& args);
    virtual bool Check(const std::string& command, const std::vector<std::string>& args) const;
    virtual std::vector<std::string> GetArgs(const std::vector<std::string>& args){ return std::vector<std::string>(); }
private:
    std::vector<std::string> m_args;
    TerminalState* m_state;
};

class ExitCommand : public Command
{
public:
    ExitCommand(TerminalState* state);
    virtual ~ExitCommand();

    virtual void Execute(const std::vector<std::string>& args);
    virtual bool Check(const std::string& command, const std::vector<std::string>& args) const;
    virtual std::vector<std::string> GetArgs(const std::vector<std::string>& args){ return std::vector<std::string>(); }
private:
    TerminalState* m_state;
};

#endif/*COMMANDS_H*/
