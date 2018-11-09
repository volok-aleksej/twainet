#include "application.h"

ApplicationTerminal::ApplicationTerminal()
{

}

ApplicationTerminal::~ApplicationTerminal()
{
}

void ApplicationTerminal::InitializeApplication()
{
    TerminalModule* module = new TerminalModule;
    AddModule(module);
    module->Init();
}

std::string ApplicationTerminal::GetAppName()
{
    return "Remote Terminal";
}

std::string ApplicationTerminal::GetDescription()
{
    return "Remote terminal for clever home";
}

void ApplicationTerminal::OnModuleCreationFailed(Twainet::Module module)
{
	Stop();
}

void ApplicationTerminal::OnServerCreationFailed(Twainet::Module module)
{
	Stop();
}
