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
        Thread::sleep(100);
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
