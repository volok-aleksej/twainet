#include "terminal_module.h"

#define MODULE_NAME	"rtch"// remote terminal for clever home

TerminalModule::TerminalModule()
: Module(MODULE_NAME)
{
}

TerminalModule::~TerminalModule()
{

}

void TerminalModule::Init()
{
    strcpy(m_userPassword.m_user, "test");
    strcpy(m_userPassword.m_pass, "test");
    Twainet::SetUsersList(m_module, &m_userPassword, 1);
    int port = 5056;//Config::GetInstance().GetLocalServerPort();
    Twainet::CreateServer(m_module, port, Twainet::IPV4, false);
}

void TerminalModule::OnModuleConnected(const Twainet::ModuleName& moduleName)
{
}

void TerminalModule::onMessage(const term_name& msg, Twainet::ModuleName path)
{
}

void TerminalModule::onMessage(const log& msg, Twainet::ModuleName path)
{
}
