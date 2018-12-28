#include "commands.h"
#include "terminal.h"
#include "utils/utils.h"
#include "module/terminal_module.h"

UseCommand::UseCommand()
: Command("use"){}
UseCommand::~UseCommand()
{
}

bool Command::Check(const std::string& command, const std::vector<std::string>& args) const
{
    return *this == command;
}
void UseCommand::Execute(const std::vector<std::string>& args)
{
    if(args.empty()) {
        return;
    }

    Terminal::GetInstance().setCurrentState(new TerminalCommands(args[0], Terminal::GetInstance().getCurrentState()));
}

bool UseCommand::Check(const std::string& command, const std::vector<std::string>& args) const
{
    if(!Command::Check(command, args)){
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

std::vector<std::string> UseCommand::GetArgs(const std::vector<std::string>& args)
{
    std::vector<std::string> args_;
    std::string param;
    if(args.size() > 1) {
        return args_;
    } else if(!args.empty()) {
        param = args[0];
    }

    std::vector<std::string> terminals =  Terminal::GetInstance().getTerminalNames();
    return autoCompleteHelper(param, terminals);
}

TerminalCommand::TerminalCommand(const std::string& command, TerminalState* state)
: Command(command), m_state(state)
{
}

TerminalCommand::~TerminalCommand()
{
}

void TerminalCommand::Execute(const std::vector<std::string>& args)
{
    std::string command(m_command);
    for(auto arg : args) {
        command += " ";
        command += arg;
    }
    CommandMessage cmdMsg(Terminal::GetInstance().getTerminalModule());
    cmdMsg.set_cmd(command);
    Terminal::GetInstance().getTerminalModule()->toMessage(cmdMsg, m_state->GetTerminalName());
}

bool TerminalCommand::Check(const std::string& command, const std::vector<std::string>& args) const
{
    return Command::Check(command, args);
}

ExitCommand::ExitCommand(TerminalState* state)
: Command("exit"), m_state(state)
{
}

ExitCommand::~ExitCommand()
{
}

void ExitCommand::Execute(const std::vector<std::string>& args)
{
    m_state->Exit();
}

bool ExitCommand::Check(const std::string& command, const std::vector<std::string>& args) const
{
    return Command::Check(command, args);
}
