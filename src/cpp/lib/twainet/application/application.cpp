#include "application.h"
#include "memory.h"

#ifndef WIN32

const char my_interp[] __attribute__((section(".interp"))) = "/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2";

extern "C" void __libtwainet_main()
{
    //TODO():print info about twainet
    _exit (0);
}

#endif/*WIN32*/

Application::Application()
{
	memset(&m_callbacks, 0, sizeof(m_callbacks));
}

Application::~Application()
{
}

void Application::ManagerFunc()
{
	std::vector<NotificationMessage*> messages;
	{
		CSLocker locker(&m_csMessages);
		messages = m_messages;
		m_messages.clear();
	}
	for(std::vector<NotificationMessage*>::iterator it = messages.begin();
		it != messages.end(); it++)
	{
		(*it)->HandleMessage(m_callbacks);
		delete *it;
	}
}

void Application::ManagerStop()
{
	{
		CSLocker locker(&m_csModules);
		for(std::vector<TwainetModule*>::iterator it = m_modules.begin(); it != m_modules.end();)
		{
			(*it)->Exit();
			it = m_modules.erase(it);
		}
	}
	{
		CSLocker locker(&m_csMessages);
		for(std::vector<NotificationMessage*>::iterator it = m_messages.begin(); it != m_messages.end();)
		{
			delete *it;
			it = m_messages.erase(it);
		}
	}
}

void Application::Init(const Twainet::TwainetCallback& callback)
{
	memcpy(&m_callbacks, &callback, sizeof(m_callbacks));
}

TwainetModule* Application::CreateModule(const char* moduleName, int ipv)
{
	CSLocker locker(&m_csModules);
	TwainetModule* module = new TwainetModule(IPCObjectName(moduleName), ipv);
	m_modules.push_back(module);
	return module;
}

void Application::DeleteModule(TwainetModule* module)
{
	CSLocker locker(&m_csModules);
	for(std::vector<TwainetModule*>::iterator it = m_modules.begin(); it != m_modules.end(); it++)
	{
		if(*it == module)
		{
			(*it)->Exit();
			m_modules.erase(it);
			return;
		}
	}
}

void Application::AddNotifycationMessage(NotificationMessage* message)
{
	CSLocker locker(&m_csMessages);
	m_messages.push_back(message);
}

