#ifndef CONSOLE_H
#define CONSOLE_H

#include <map>
#include <string>
#include <vector>
#include <termios.h>
#include "thread_lib/thread/thread_impl.h"

class Console : public ThreadImpl
{
public:
    enum AccumTypeChar
    {
        ARROW_UP,
        ARROW_DOWN,
        ARROW_LEFT,
        ARROW_RIGHT,
        DELETE,
        END,
        HOME,
        PAGEUP,
        PAGEDOWN,
        INSERT
    };
    Console();
    virtual ~Console();
    void Init();
    void DeInit();
    bool Write(const std::string& log);
    bool Read(std::string& buf);
    void SetTermName(const std::string& termName);
protected:
    virtual void ThreadFunc();
    virtual void OnStop(){}
    virtual void OnStart(){}
    virtual void Stop(){}
protected:
    void printName();
    void clearLine();
    int checkAccumeChars(AccumTypeChar* ac_char);
    bool useChar(char simb, std::string& buf);
private:
    std::string m_command;
    std::string m_termName;
    FILE* m_stream;
    CriticalSection m_cs;
    std::vector<char> m_accumchars;
    std::vector<std::string> m_history;
    int m_historyCounter;
    bool m_tab;
    int m_carpos;
    struct termios old;
    std::map<AccumTypeChar, std::vector<char> > m_templates;
};

#endif/*CONSOLE_H*/
