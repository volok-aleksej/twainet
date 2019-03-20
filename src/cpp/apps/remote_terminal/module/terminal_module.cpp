#include "terminal_module.h"
#include <application/application.h>
#include <terminal/terminal.h>
#include <terminal/commands.h>

#define MODULE_NAME	"rtch"// remote terminal for clever home

TerminalModule::TerminalModule()
: Module(MODULE_NAME)
{
    Terminal::GetInstance().addTerminalModule(this);
    AddMessage(new TermNameMessage(this));
    AddMessage(new LogMessage(this));
    AddMessage(new CommandListMessage(this));
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

bool TerminalModule::toMessage(const DataMessage& msg, const std::string& termName)
{
    CSLocker lock(&m_cs);
    for(std::map<Twainet::ModuleName, std::string>::iterator it = m_terminalMap.begin();
        it != m_terminalMap.end(); it++) {
        if(it->second == termName) {
            return Module::toMessage(msg, it->first);
        }
    }
    return false;
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

void TerminalModule::onMessage(const command_list& msg, Twainet::ModuleName path)
{
    std::string terminalName;
    {
        CSLocker lock(&m_cs);
        std::map<Twainet::ModuleName, std::string>::iterator it = m_terminalMap.find(path);
        if(it != m_terminalMap.end()) {
            terminalName = it->second;
        }
    }
    if(Terminal::GetInstance().getCurrentState()->GetTerminalName() == terminalName) {
        for(int i = 0; i < msg.cmd_size(); i++) {
            std::vector<std::string> args;
            for(int j = 0; j < msg.cmd(i).args_size(); j++) {
                args.push_back(msg.cmd(i).args(j));
            }
            Terminal::GetInstance().getCurrentState()->AddCommand(new TerminalCommand(msg.cmd(i).cmd(), args, Terminal::GetInstance().getCurrentState()));
        }
    }
}
