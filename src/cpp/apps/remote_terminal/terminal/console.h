#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>
#include <termios.h>

class Console
{
public:
    Console();
    virtual ~Console();
    void Init();
    void DeInit();
    bool Write(const std::string& log);
    bool Read(std::string& buf);
    void SetTermName(const std::string& termName);
protected:
    void printName();
    void ClearLine();
private:
    std::string m_command;
    std::string m_termName;
    FILE* m_stream;
    struct termios old;
};

#endif/*CONSOLE_H*/
