#include "application.h"
#include "memory.h"

Application::Application()
	: m_isExit(false)
{
	memset(&m_callbacks, 0, sizeof(m_callbacks));
}

Application::~Application()
{
	Stop();
	Join();
}

void Application::ThreadFunc()
{
	while(!m_isExit)
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
		
		sleep(200);
	}
}

void Application::OnStop()
{
	{
		CSLocker locker(&m_csModules);
		for(std::vector<TwainetModule*>::iterator it = m_modules.begin(); it != m_modules.end();)
		{
			(*it)->Exit();
			delete *it;
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

void Application::OnStart()
{
}

void Application::Stop()
{
	m_isExit = true;
}

void Application::Init(const Twainet::TwainetCallback& callback)
{
	memcpy(&m_callbacks, &callback, sizeof(m_callbacks));
}

TwainetModule* Application::CreateModule(const Twainet::ModuleName& moduleName)
{
	CSLocker locker(&m_csModules);
	TwainetModule* module = new TwainetModule(IPCObjectName(moduleName.m_name, moduleName.m_host, moduleName.m_suffix));
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
			delete *it;
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

