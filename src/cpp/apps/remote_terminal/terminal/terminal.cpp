#include <stdio.h>
#include <algorithm>
#include "terminal.h"
#include "utils/utils.h"
#include "module/terminal_module.h"

Terminal::Terminal()
: m_currentTerminal(&m_rootCommand)
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
        if(!m_console.Read(command)) break;
        std::vector<std::string> args = CommonUtils::DelimitQString(command, " ");
        if(args.empty()) {
            continue;
        }
        command = args[0];
        args.erase(args.begin());
        if(m_currentTerminal->Check(command, args)){
            m_currentTerminal->Execute(args);
        }
    };
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
    if(m_currentTerminal != &m_rootCommand && *m_currentTerminal == terminalName) {
        m_currentTerminal->Exit();
    }
}

void Terminal::setCurrentTerminal(Command* term)
{
    m_currentTerminal = term;
    m_console.SetTermName(m_currentTerminal->GetCurrentTerminalName());
}

Command* Terminal::getCurrentTerminal()
{
    return m_currentTerminal;
}

void Terminal::addTerminalModule(TerminalModule* module)
{
    m_terminalModule = module;
}

std::vector<std::string> Terminal::getTerminalNames()
{
    std::vector<std::string> names;
    if(m_terminalModule) {
        names = m_terminalModule->getTerminalNames();
    }
    return names;
}
