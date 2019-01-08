#include <stdio.h>
#include <algorithm>
#include "terminal.h"
#include "utils/utils.h"
#include "module/terminal_module.h"

Terminal::Terminal()
: m_currentState(&m_rootState)
, m_terminalModule()
{
    Start();
}

Terminal::~Terminal()
{
    Join();
}

void Terminal::ThreadFunc()
{
    m_console.Init();
    while(!IsStop()) {
        std::string command;
        Thread::sleep(50);
        if(!m_console.Read(command)) continue;
        std::vector<std::string> args = CommonUtils::DelimitQString(command, " ");
        if(args.empty()) {
            continue;
        }
        command = args[0];
        args.erase(args.begin());
        if(m_currentState->Check(command, args)) {
            m_currentState->Execute(command, args);
        }
    }
    m_console.DeInit();
}

void Terminal::OnStop()
{
}

void Terminal::OnStart()
{
}

void Terminal::Stop()
{
}

void Terminal::log(const std::string& termName, uint64_t time, const std::string& data)
{
    std::string log(termName);
    log += ":";
    log += data;
    m_console.Write(log);
}

void Terminal::onTerminalDisconnected(const std::string& terminalName)
{
    if(m_currentState && m_currentState->GetTerminalName() == terminalName) {
        m_currentState->Exit();
    }
}

bool Terminal::autoComplete(std::string& command)
{
    std::string cmd;
    std::vector<std::string> args = CommonUtils::DelimitQString(command, " ");
    if(!args.empty()) {
        cmd = args[0];
        args.erase(args.begin());
    }

    std::vector<std::string> usewords;
    if(command.empty() || (args.empty() && command.back() != ' ')) {
        std::vector<std::string> commands = m_currentState->GetCommands();
        usewords = autoCompleteHelper(cmd, commands);
        if(usewords.size() == 1) {
            command = usewords[0] + " ";
            return true;
        } else {
            std::string msg;
            for(auto word_ : usewords) {
                msg += word_;
                msg += " ";
            }
            if(!msg.empty()) {
                log("", CommonUtils::GetCurrentTime(), msg);
            }
        }
    } else {
        usewords = m_currentState->GetArgs(cmd, args);
        if(usewords.size() == 1) {
            args.back() = usewords[0];
            command = cmd + " ";
            for(auto arg : args) {
                command += arg + " ";
            }
            return true;
        } else {
            std::string msg;
            for(auto word_ : usewords) {
                msg += word_;
                msg += " ";
            }
            if(!msg.empty()) {
                log("", CommonUtils::GetCurrentTime(), msg);
            }
        }
    }
    return false;
}

void Terminal::setCurrentState(TerminalState* state)
{
    m_currentState = state;
    m_console.SetTermName(state->GetTerminalName());
}

TerminalState* Terminal::getCurrentState()
{
    return m_currentState;
}

void Terminal::addTerminalModule(TerminalModule* module)
{
    m_terminalModule = module;
}

TerminalModule* Terminal::getTerminalModule()
{
    return m_terminalModule;
}

std::vector<std::string> Terminal::getTerminalNames()
{
    std::vector<std::string> names;
    if(m_terminalModule) {
        names = m_terminalModule->getTerminalNames();
    }
    return names;
}
