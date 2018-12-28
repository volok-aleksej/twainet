#ifndef TERMINAL_H
#define TERMINAL_H

#include <string>
#include <stdint.h>
#include "console.h"
#include "terminal_state.h"
#include "auto_complete_helper.h"
#include "include/module.h"
#include "thread_lib/common/thread_singleton.h"

class TerminalModule;

class Terminal : public ThreadSingleton<Terminal>, public AutoCompleteHelper
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

    void autoComplete(std::string& command);

    void setCurrentState(TerminalState* state);
    TerminalState* getCurrentState();

    void addTerminalModule(TerminalModule* module);
    TerminalModule* getTerminalModule();
    std::vector<std::string> getTerminalNames();
private:
    Console m_console;
    UseTerminal m_rootState;
    TerminalState* m_currentState;
    TerminalModule* m_terminalModule;
};

#endif/*TERMINAL_H*/
