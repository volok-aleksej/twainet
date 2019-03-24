#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "console.h"
#include "terminal.h"

Console::Console()
: m_stream(0), old{0}, m_historyCounter(0), m_tab(false), m_carpos(0)
{
    m_templates.insert(std::make_pair(ARROW_UP, std::vector<char>{27, '[', 'A'}));
    m_templates.insert(std::make_pair(ARROW_DOWN, std::vector<char>{27, '[', 'B'}));
    m_templates.insert(std::make_pair(ARROW_LEFT, std::vector<char>{27, '[', 'D'}));
    m_templates.insert(std::make_pair(ARROW_RIGHT, std::vector<char>{27, '[', 'C'}));
    m_templates.insert(std::make_pair(DELETE, std::vector<char>{27, '[', '3', '~'}));
    m_templates.insert(std::make_pair(END, std::vector<char>{27, '[', 'F'}));
    m_templates.insert(std::make_pair(HOME, std::vector<char>{27, '[', 'H'}));
    m_templates.insert(std::make_pair(PAGEDOWN, std::vector<char>{27, '[', '6', '~'}));
    m_templates.insert(std::make_pair(PAGEUP, std::vector<char>{27, '[', '5', '~'}));
    m_templates.insert(std::make_pair(INSERT, std::vector<char>{27, '[', '2', '~'}));
}

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
    Start();
}


void Console::DeInit()
{
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(stdin->_fileno, TCSADRAIN, &old) < 0)
            perror ("tcsetattr ~ICANON");
    close(m_stream->_fileno);
}

Console::~Console()
{
    Join();
}

void Console::ThreadFunc()
{
    unsigned char ch;
    do {
        if(read(m_stream->_fileno, &ch, 1) < 0) break;
        CSLocker lock(&m_cs);
        m_accumchars.push_back(ch);
    } while(ch != -1);
}

bool Console::Write(const std::string& log)
{
    clearLine();
    if(m_stream) {
        fprintf(m_stream, "%s\n", log.c_str());
        printName();
        fprintf(m_stream, "%s", m_command.c_str());
        for(int i = m_command.size(); i > m_carpos; i--) {
            fprintf(m_stream, "\b");
        }
        fflush(m_stream);
    }
    return true;
}

bool Console::Read(std::string& buf)
{
    if(!m_stream) {
        return false;
    }

    AccumTypeChar ac_char;
    CSLocker lock(&m_cs);
    do {
        int ret = checkAccumeChars(&ac_char);
        if(ret != 0) {
                if(m_accumchars.size() > 0) {
                    char ch_ = m_accumchars[0];
                    m_accumchars.erase(m_accumchars.begin());
                    if(useChar(ch_, buf)) return true;
                }
                return false;
        } else if(ac_char == ARROW_UP){
            if(!m_history.empty() && m_historyCounter < m_history.size()) {
                clearLine();
                printName();
                m_historyCounter++;
                m_command = *(m_history.end() - m_historyCounter);
                m_carpos = m_command.size();
                fprintf(m_stream, "%s", m_command.c_str());
                fflush(m_stream);
            }
        } else if(ac_char == ARROW_DOWN){
            if(!m_history.empty() && m_historyCounter != 1) {
                m_historyCounter--;
                clearLine();
                printName();
                m_command = *(m_history.end() - m_historyCounter);
                m_carpos = m_command.size();
                fprintf(m_stream, "%s", m_command.c_str());
                fflush(m_stream);
            }
        } else if(ac_char == ARROW_LEFT) {
            if(m_carpos != 0) {
                m_carpos--;
                fprintf(m_stream, "\b");
                fflush(m_stream);
            }
        } else if(ac_char == ARROW_RIGHT) {
            if(m_carpos < m_command.size()) {
                fprintf(m_stream, "%c", *(m_command.c_str() + m_carpos));
                m_carpos++;
                fflush(m_stream);
            }
        } else  if(ac_char == DELETE) {
            if(m_carpos < m_command.size()) {
                m_command.erase(m_command.begin() + m_carpos);
                fprintf(m_stream, "%s", m_command.c_str() + m_carpos);
                fprintf(m_stream, " \b");
                for(int i = m_carpos; i < m_command.size(); i++) {
                    fprintf(m_stream, "\b");
                }
                fflush(m_stream);
            }
        } else  if(ac_char == END) {
            for(int i = m_carpos; i < m_command.size(); i++) {
                fprintf(m_stream, "%c", m_command[i]);
            }
            fflush(m_stream);
            m_carpos = m_command.size();
        } else  if(ac_char == HOME) {
            for(int i = 0; i < m_command.size(); i++) {
                fprintf(m_stream, "\b");
            }
            fflush(m_stream);
            m_carpos = 0;
        }
        m_accumchars.erase(m_accumchars.begin(), m_accumchars.begin() + m_templates[ac_char].size());
    } while(!m_accumchars.empty());
    m_accumchars.clear();
    return false;
}

bool Console::useChar(char ch, std::string& buf)
{
    if(ch == '\t' && !m_tab) {
        m_tab = true;
        return false;
    } else if(ch == '\t' && m_tab) {
        m_tab = false;
        std::string command(m_command.begin(), m_command.begin() + m_carpos);
        if(Terminal::GetInstance().autoComplete(command)) {
            clearLine();
            printName();
            m_command = command;
            m_carpos = m_command.size();
            fprintf(m_stream, "%s", m_command.c_str());
            fflush(m_stream);
        }
        return false;
    }

    m_tab = false;
    if(ch == '\n') {
        buf = m_command;
        m_command = "";
        m_carpos = 0;
        fprintf(m_stream, "\n");
        printName();
        fflush(m_stream);
        m_historyCounter = 0;
        if(!buf.empty())
            m_history.push_back(buf);
        return true;
    } else if(ch == 27) {                        // escape
        clearLine();
        printName();
        fflush(m_stream);
        m_command = "";
        m_carpos = 0;
    } else if(ch == '\r') {
    } else if(ch == 127 && m_command.length()) { // '\b'
        m_command.erase(m_command.begin() + m_carpos - 1);
        m_carpos--;
        fprintf(m_stream, "\b");
        fprintf(m_stream, "%s", m_command.c_str() + m_carpos);
        fprintf(m_stream, " \b");
        for(int i = m_carpos; i < m_command.size(); i++) {
            fprintf(m_stream, "\b");
        }
        fflush(m_stream);
    } else if(ch > 31 && ch < 127) {
        fprintf(m_stream, "%c", ch);
        fprintf(m_stream, "%s", m_command.c_str() + m_carpos);
        m_command.insert(m_command.begin() + m_carpos, ch);
        m_carpos++;
        for(int i = m_carpos; i < m_command.size(); i++) {
            fprintf(m_stream, "\b");
        }
        fflush(m_stream);
    }
    return false;
}

void Console::SetTermName(const std::string& termName)
{
    clearLine();
    m_termName = termName;
    printName();
    m_carpos = 0;
    fflush(m_stream);
}

void Console::printName()
{
    fprintf(m_stream, "#%s:", m_termName.c_str());
}

void Console::clearLine()
{
    if(!m_stream) {
        return;
    }
       
    for(int i = m_carpos + 2 + m_termName.size(); i < m_command.length() + 2 + m_termName.size(); i++){
        fprintf(m_stream, " ");
    }

    for(int i= 0; i < m_command.length() + 2 + m_termName.size(); i++){
        fprintf(m_stream, "\b \b");
    }
}

int Console::checkAccumeChars(AccumTypeChar* ac_char)
{
    for(auto tmpl : m_templates) {
        int size = m_accumchars.size();
        if(size > tmpl.second.size()) {
            size = tmpl.second.size();
        }
        if(memcmp(tmpl.second.data(), m_accumchars.data(), size) == 0) {
            if(tmpl.second.size() == size) {
                *ac_char = tmpl.first;
                return 0;
            }
            return -1;
        }
    }
    return 1;
}
