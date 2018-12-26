#include "terminal_state.h"
#include "terminal.h"
#include "commands.h"
#include <utils/utils.h>

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
        const_cast<CommandChecker&>(cmd).command = const_cast<Command*>(command);
    }
}

bool TerminalState::ExecuteCommand(const CommandChecker& cmd, const Command* command)
{
    if(*command == cmd.cmd)
    {
        const_cast<Command*>(command)->Execute(cmd.args);
    }

    return true;
}

void TerminalState::AddCommand(Command* command)
{
    m_commands.AddObject(command);
}

bool TerminalState::Check(const std::string& command, const std::vector<std::string>& args)
{
    CommandChecker commandChecker{command, args, 0};
    m_commands.ProcessingObjects(Ref(this, &TerminalState::CheckCommand, commandChecker));
    if(commandChecker.command == 0) {
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
    CommandChecker commandChecker{command, args, 0};
    m_commands.ProcessingObjects(Ref(this, &TerminalState::ExecuteCommand, commandChecker));
}

UseTerminal::UseTerminal()
{
    AddCommand(new UseCommand);
}

TerminalCommands::TerminalCommands(const std::string& termName, TerminalState* parent)
: m_terminalName(termName), m_parent(parent)
{
    AddCommand(new ExitCommand(this));
}

TerminalCommands::~TerminalCommands()
{
}

void TerminalCommands::Exit()
{
    Terminal::GetInstance().setCurrentState(m_parent);
    delete this;
}
