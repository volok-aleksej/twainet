#include <stdio.h>
#include <unistd.h>
#include "console.h"

Console::Console()
: m_stream(0), old{0} {}

void Console::Init()
{
    m_stream = stdout;
    
    if (tcgetattr(stdin->_fileno, &old) < 0)
            perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(stdin->_fileno, TCSANOW, &old) < 0)
            perror("tcsetattr ICANON");
    
    printName();
    fflush(m_stream);
    
}


void Console::DeInit()
{
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(stdin->_fileno, TCSADRAIN, &old) < 0)
            perror ("tcsetattr ~ICANON");
}

Console::~Console()
{
}

bool Console::Write(const std::string& log)
{
    ClearLine();
    if(m_stream) {
        fprintf(m_stream, "%s\n", log.c_str());
        printName();
        fprintf(m_stream, "%s", m_command.c_str());
        fflush(m_stream);
    }
    return true;
}

bool Console::Read(std::string& buf)
{
    if(!m_stream) {
        return false;
    }
        
    unsigned char ch;
    do {
        if(read(m_stream->_fileno, &ch, 1) < 0) break;
        if(ch == '\n')
        {
            buf = m_command;
            m_command = "";
            fprintf(m_stream, "\n");
            printName();
            fflush(m_stream);
            return true;
        } else if(ch == 27){                        // escape
            ClearLine();
            printName();
            fflush(m_stream);
            m_command = "";
        } else if(ch == '\t'){
        } else if(ch == '\r'){
        } else if(ch == 127 && m_command.length()){ // '\b'
            m_command.erase(m_command.end() - 1);
            fprintf(m_stream, "\b \b");
            fflush(m_stream);
        } else if(ch > 31 && ch < 127){
            fprintf(m_stream, "%c", ch);
            fflush(m_stream);
            m_command.push_back(ch);
        }
    } while(ch != -1);
    return false;
}

void Console::printName()
{
    fprintf(m_stream, "#%s:", m_termName.c_str());
}

void Console::ClearLine()
{
    if(!m_stream) {
        return;
    }
       
    for(int i= 0; i < m_command.length() + 2 + m_termName.size(); i++){
        fprintf(m_stream, "\b \b");
    }
}