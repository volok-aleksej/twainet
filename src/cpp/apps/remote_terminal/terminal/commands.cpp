#include "commands.h"
#include "terminal.h"
#include <utils/utils.h>

UseCommand::UseCommand()
: Command("use"), m_termCommand(0){}
UseCommand::~UseCommand()
{
    if(m_termCommand) {
        delete m_termCommand;
    }
}

void UseCommand::Execute(const std::vector<std::string>& args)
{
    if(args.empty()) {
        return;
    }

    m_termCommand = new TerminalCommand(args[0], this);
    Terminal::GetInstance().setCurrentTerminal(m_termCommand);
}

bool UseCommand::Check(const std::string& command, const std::vector<std::string>& args)
{
    if(*this != command) {
        std::string msg("invalid command '");
        msg += command;
        msg += "'";
        Terminal::GetInstance().log("", CommonUtils::GetCurrentTime(), msg);
        return false;
    }

    if(args.empty()) {
        std::string msg("invalid terminal name - name is absent");
        Terminal::GetInstance().log("", CommonUtils::GetCurrentTime(), msg);
        return false;
    } else if(args.size() > 1) {
        std::string msg("invalid attributes - terminal name must be single");
        Terminal::GetInstance().log("", CommonUtils::GetCurrentTime(), msg);
        return false;
    }

    std::vector<std::string> names = Terminal::GetInstance().getTerminalNames();
    std::vector<std::string>::iterator it = std::find(names.begin(), names.end(), args[0]);
    if(it == names.end()) {
        std::string msg("invalid terminal name - terminal with name '");
        msg += args[0];
        msg += "' is absent";
        Terminal::GetInstance().log("", CommonUtils::GetCurrentTime(), msg);
        return false;
    }
    return true;
}

std::string UseCommand::GetCurrentTerminalName()
{
    return "";
}

void UseCommand::Exit()
{
    Terminal::GetInstance().setCurrentTerminal(this);
    delete m_termCommand;
}

TerminalCommand::TerminalCommand(const std::string& terminalName, Command* parent)
: Command(terminalName), m_parentCommand(parent)
{
}

TerminalCommand::~TerminalCommand()
{
}

void TerminalCommand::Execute(const std::vector<std::string>& args)
{
}

bool TerminalCommand::Check(const std::string& command, const std::vector<std::string>& args)
{
    return false;
}

std::string TerminalCommand::GetCurrentTerminalName()
{
    return m_command;
}

void TerminalCommand::Exit()
{
    m_parentCommand->Exit();
}
