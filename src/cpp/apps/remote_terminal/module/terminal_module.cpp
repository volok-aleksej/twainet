#include "terminal_module.h"
#include <application/application.h>
#include <terminal/terminal.h>

#define MODULE_NAME	"rtch"// remote terminal for clever home

TerminalModule::TerminalModule()
: Module(MODULE_NAME)
{
    Terminal::GetInstance().addTerminalModule(this);
    AddMessage(new TermNameMessage(this));
    AddMessage(new LogMessage(this));
}

TerminalModule::~TerminalModule()
{

}

void TerminalModule::Init()
{
    strcpy(m_userPassword.m_user, "test");
    strcpy(m_userPassword.m_pass, "test");
    Twainet::SetUsersList(m_module, &m_userPassword, 1);
    int port = 5200;//Config::GetInstance().GetLocalServerPort();
    Twainet::CreateServer(m_module, port, Twainet::IPV4, false);
}

std::vector<std::string> TerminalModule::getTerminalNames()
{
    std::vector<std::string> names;
    CSLocker lock(&m_cs);
    for(std::map<Twainet::ModuleName, std::string>::iterator it = m_terminalMap.begin();
        it != m_terminalMap.end(); it++) {
        names.push_back(it->second);
    }
    return names;
}

void TerminalModule::OnModuleConnected(const Twainet::ModuleName& moduleName)
{
}

void TerminalModule::OnModuleDisconnected(const Twainet::ModuleName & moduleName)
{
    CSLocker lock(&m_cs);
    std::map<Twainet::ModuleName, std::string>::iterator it = m_terminalMap.find(moduleName);
    if(it != m_terminalMap.end()) {
        Terminal::GetInstance().onTerminalDisconnected(it->second);
        m_terminalMap.erase(it);
    }
}

void TerminalModule::onMessage(const term_name& msg, Twainet::ModuleName path)
{
    {
        CSLocker lock(&m_cs);
        m_terminalMap.insert(std::make_pair(path, msg.name()));
    }
    Terminal::GetInstance().log(msg.name(), 0, "connect terminal");
}

void TerminalModule::onMessage(const log& msg, Twainet::ModuleName path)
{
    std::string terminalName;
    {
        CSLocker lock(&m_cs);
        std::map<Twainet::ModuleName, std::string>::iterator it = m_terminalMap.find(path);
        if(it != m_terminalMap.end()) {
            terminalName = it->second;
        }
    }
    Terminal::GetInstance().log(terminalName, msg.time(), msg.data());
}
