#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <vector>

class Terminal;
class TerminalCommand;

class Command
{
public:
    explicit Command(const std::string& command)
    : m_command(command){}
    virtual ~Command(){}

    virtual void Execute(const std::vector<std::string>& args) = 0;
    virtual bool Check(const std::string& command, const std::vector<std::string>& args) = 0;
    virtual std::string GetCurrentTerminalName() = 0;
    virtual void Exit() = 0;

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
    virtual bool Check(const std::string& command, const std::vector<std::string>& args);
    virtual std::string GetCurrentTerminalName();
    virtual void Exit();
private:
    TerminalCommand* m_termCommand;
};

class TerminalCommand : public Command
{
public:
    TerminalCommand(const std::string& terminalName, Command* parent);
    virtual ~TerminalCommand();

    virtual void Execute(const std::vector<std::string>& args);
    virtual bool Check(const std::string& command, const std::vector<std::string>& args);
    virtual std::string GetCurrentTerminalName();
    virtual void Exit();
private:
    Command* m_parentCommand;
//    std::vector<Command*> m_childCommand;
};

#endif/*COMMANDS_H*/
