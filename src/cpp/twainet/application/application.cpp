#include "application.h"
#include "memory.h"

Application::Application()
	: m_isExit(false)
{
	memset(&m_callbacks, 0, sizeof(m_callbacks));
	Start();
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
		sleep(100);
	}
}

void Application::OnStop()
{
	CSLocker locker(&m_cs);
	for(std::vector<TwainetModule*>::iterator it = m_modules.begin(); it != m_modules.end();)
	{
		(*it)->Exit();
		delete *it;
		it = m_modules.erase(it);
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

TwainetModule* Application::CreateModule(const char* moduleName)
{
	CSLocker locker(&m_cs);
	TwainetModule* module = new TwainetModule(IPCObjectName::GetIPCName(moduleName));
	m_modules.push_back(module);
	return module;
}

void Application::DeleteModule(TwainetModule* module)
{
	CSLocker locker(&m_cs);
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
