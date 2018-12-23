#ifndef TERMINAL_H
#define TERMINAL_H

#include <string>
#include <stdint.h>
#include "lib/thread_lib/common/thread_singleton.h"
#include "console.h"

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
private:
    Console m_console;
};

#endif/*TERMINAL_H*/