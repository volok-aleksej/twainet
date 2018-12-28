class TermState;
#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <vector>
#include "terminal_state.h"

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

    bool operator == (const std::string& command) const {
        return m_command == command;
    }
    bool operator != (const std::string& command) const {
        return m_command != command;
    }
protected:
    std::string m_command;
};

class UseCommand : public Command
{
public:
    UseCommand();
    virtual ~UseCommand();

    virtual void Execute(const std::vector<std::string>& args);
    virtual bool Check(const std::string& command, const std::vector<std::string>& args) const;
};

class TerminalCommand : public Command
{
public:
    TerminalCommand(const std::string& command, TerminalState* state);
    virtual ~TerminalCommand();

    virtual void Execute(const std::vector<std::string>& args);
    virtual bool Check(const std::string& command, const std::vector<std::string>& args) const;
private:
    TerminalState* m_state;
};

class ExitCommand : public Command
{
public:
    ExitCommand(TerminalState* state);
    virtual ~ExitCommand();

    virtual void Execute(const std::vector<std::string>& args);
    virtual bool Check(const std::string& command, const std::vector<std::string>& args) const;
private:
    TerminalState* m_state;
};

#endif/*COMMANDS_H*/
