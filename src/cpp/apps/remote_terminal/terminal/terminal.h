#ifndef TERMINAL_H
#define TERMINAL_H

#include <string>
#include <stdint.h>
#include "console.h"
#include "commands.h"
#include "include/module.h"
#include "thread_lib/common/thread_singleton.h"

class TerminalModule;

class Terminal : public ThreadSingleton<Terminal>
{
    friend class Singleton<Terminal>;
    Terminal();
public:
    ~Terminal();
    
    virtual void ThreadFunc();
    virtual void OnStop();
    virtual void OnStart();
    virtual void Stop();
    
    void log(const std::string& termName, uint64_t time, const std::string& data);

    void onTerminalDisconnected(const std::string& terminalName);

    void setCurrentTerminal(Command* term);
    Command* getCurrentTerminal();

    void addTerminalModule(TerminalModule* module);
    std::vector<std::string> getTerminalNames();
private:
    Console m_console;
    UseCommand m_rootCommand;
    Command* m_currentTerminal;
    TerminalModule* m_terminalModule;
};

#endif/*TERMINAL_H*/
