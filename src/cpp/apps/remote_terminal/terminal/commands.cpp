#include "commands.h"
#include "terminal.h"
#include "utils/utils.h"
#include "module/terminal_module.h"

UseCommand::UseCommand()
: Command("use"){}
UseCommand::~UseCommand()
{
}

void UseCommand::Execute(const std::vector<std::string>& args)
{
    if(args.empty()) {
        std::string msg("invalid terminal name - name is absent");
        Terminal::GetInstance().log("", CommonUtils::GetCurrentTime(), msg);
        return;
    } else if(args.size() > 1) {
        std::string msg("invalid attributes - terminal name must be single");
        Terminal::GetInstance().log("", CommonUtils::GetCurrentTime(), msg);
        return;
    }

    std::vector<std::string> names = Terminal::GetInstance().getTerminalNames();
    std::vector<std::string>::iterator it = std::find(names.begin(), names.end(), args[0]);
    if(it == names.end()) {
        std::string msg("invalid terminal name - terminal with name '");
        msg += args[0];
        msg += "' is absent";
        Terminal::GetInstance().log("", CommonUtils::GetCurrentTime(), msg);
        return;
    }

    Terminal::GetInstance().setCurrentState(new TerminalCommands(args[0], Terminal::GetInstance().getCurrentState()));
}

std::vector<std::string> UseCommand::GetArgs(const std::vector<std::string>& args)
{
    std::vector<std::string> args_;
    std::string param = args.empty() ? "" : args.back();
    if(args.size() <= 1) {
        args_ =  Terminal::GetInstance().getTerminalNames();
    }
    return autoCompleteHelper(param, args_);
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
    if(args.size() > 1) {
        std::string msg("invalid attributes: exit is command without arguments");
        Terminal::GetInstance().log("", CommonUtils::GetCurrentTime(), msg);
        return;
    }
    m_state->Exit();
}

