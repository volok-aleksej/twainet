#include "terminal_state.h"
#include "terminal.h"
#include "commands.h"
#include "utils/utils.h"
#include <module/terminal_module.h>

TerminalState::TerminalState()
{
}

TerminalState::~TerminalState()
{
    m_commands.ProcessingObjects(Ref(this, &TerminalState::DeleteCommand));
}

bool TerminalState::DeleteCommand(const Command* command)
{
    delete const_cast<Command*>(command);
    return true;
}


bool TerminalState::CheckCommand(const CommandChecker& cmd, const Command* command)
{
    if(command->Check(cmd.cmd, cmd.args))
    {
        const_cast<CommandChecker&>(cmd).find = true;
    }
    return true;
}

bool TerminalState::ExecuteCommand(const CommandChecker& cmd, const Command* command)
{
    if(*command == cmd.cmd)
    {
        const_cast<Command*>(command)->Execute(cmd.args);
    }

    return true;
}

bool TerminalState::GetCommand(const std::vector<std::string>& cmd, const Command* command)
{
    const_cast<std::vector<std::string>&>(cmd).push_back(command->GetCommand());
    return true;
}

bool TerminalState::GetArgsCommand(const TerminalState::CommandChecker& cmd, const Command* command)
{
    if(*command == cmd.cmd)
    {
        const_cast<CommandChecker&>(cmd).avail_args = const_cast<Command*>(command)->GetArgs(cmd.args);
    }
    return true;
}

void TerminalState::AddCommand(Command* command)
{
    m_commands.AddObject(command);
}

bool TerminalState::Check(const std::string& command, const std::vector<std::string>& args)
{
    std::vector<std::string> args_;
    CommandChecker commandChecker{command, args, false, args_};
    m_commands.ProcessingObjects(Ref(this, &TerminalState::CheckCommand, commandChecker));
    if(!commandChecker.find) {
        std::string msg("invalid command '");
        msg += command;
        msg += "'";
        Terminal::GetInstance().log("", CommonUtils::GetCurrentTime(), msg);
        return false;
    }
    return true;
}

void TerminalState::Execute(const std::string& command, const std::vector<std::string>& args)
{
    std::vector<std::string> args_;
    CommandChecker commandChecker{command, args, false, args_};
    m_commands.ProcessingObjects(Ref(this, &TerminalState::ExecuteCommand, commandChecker));
}

std::vector<std::string> TerminalState::GetCommands()
{
    std::vector<std::string> commands;
    m_commands.ProcessingObjects(Ref(this, &TerminalState::GetCommand, commands));
    return commands;
}

std::vector<std::string> TerminalState::GetArgs(const std::string& command, const std::vector<std::string>& args)
{
    std::vector<std::string> args_;
    CommandChecker commandChecker{command, args, false, args_};
    m_commands.ProcessingObjects(Ref(this, &TerminalState::GetArgsCommand, commandChecker));
    return args_;
}

UseTerminal::UseTerminal()
{
    AddCommand(new UseCommand);
}

TerminalCommands::TerminalCommands(const std::string& termName, TerminalState* parent)
: m_terminalName(termName), m_parent(parent)
{
    AddCommand(new ExitCommand(this));
    GetCommandListMessage gclMsg(Terminal::GetInstance().getTerminalModule());
    Terminal::GetInstance().getTerminalModule()->toMessage(gclMsg, m_terminalName);
}

TerminalCommands::~TerminalCommands()
{
}

void TerminalCommands::Exit()
{
    Terminal::GetInstance().setCurrentState(m_parent);
    delete this;
}
