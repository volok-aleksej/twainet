#include <stdio.h>
#include "terminal.h"

Terminal::Terminal()
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