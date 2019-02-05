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
    for(auto& cmd : m_commands)
        delete cmd;
}

void TerminalState::AddCommand(Command* command)
{
    m_commands.push_back(command);
}


bool TerminalState::Execute(const std::string& command, const std::vector<std::string>& args)
{
    std::vector<std::string> args_;
    for(auto& cmd : m_commands) {
        if(command == cmd->GetCommand())
        {
            cmd->Execute(args);
            return true;
        }
    }

    return false;
}

std::vector<std::string> TerminalState::GetArgs(const std::vector<std::string>& args)
{
    std::string command = args.empty() ? "" : args[0];
    std::vector<std::string> commands;
    for(auto& cmd : m_commands) {
        if(command == cmd->GetCommand())
        {
            std::vector<std::string> args_(args.begin() + 1, args.end());
            return cmd->GetArgs(args_);
        }

        commands.push_back(cmd->GetCommand());
    }

    if(args.size() <= 1) {
        return autoCompleteHelper(command, commands);
    }

    return std::vector<std::string>();
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

std::vector<std::string> TerminalCommands::GetArgs(const std::vector<std::string>& args)
{
    std::vector<std::string> out_args = TerminalState::GetArgs(args);
    GetNextCommandArgsMessage msg(Terminal::GetInstance().getTerminalModule());
    NextCommandArgsMessage resp(Terminal::GetInstance().getTerminalModule());
    for(auto& arg : args) {
        msg.add_args(arg);
    }
    if(Terminal::GetInstance().getTerminalModule()->toTermMessage(msg, m_terminalName, resp))
    {
        for(int i = 0; i < resp.args_size(); i++) {
            out_args.push_back(resp.args(i));
        }
    }
    return out_args;
}

bool TerminalCommands::Execute(const std::string& command, const std::vector<std::string>& args)
{
    if(TerminalState::Execute(command, args)) {
        return true;
    }


}

void TerminalCommands::Exit()
{
    Terminal::GetInstance().setCurrentState(m_parent);
    delete this;
}
