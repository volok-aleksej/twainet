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
: m_bInit(false)
{
	memset(&m_callbacks, 0, sizeof(m_callbacks));
    	Start();
}

Application::~Application()
{
}

void Application::ThreadFunc()
{
    while(!IsStop()) {
        std::vector<NotificationMessage*> messages;
        if(m_semaphore.Wait(200) == Semaphore::SUCCESS) {
            CSLocker locker(&m_csMessages);
            messages = m_messages;
            m_messages.clear();
            m_semaphore.Release();
        }
        
        for(std::vector<NotificationMessage*>::iterator it = messages.begin();
            it != messages.end(); it++)
        {
            (*it)->HandleMessage(m_callbacks);
            delete *it;
        }
    }
}

void Application::OnStop()
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

void Application::OnStart()
{
}

void Application::Stop()
{
    m_semaphore.Set();
}

void Application::Init(const Twainet::TwainetCallback& callback)
{
	memcpy(&m_callbacks, &callback, sizeof(m_callbacks));
    	m_bInit = true;
}

bool Application::IsInited()
{
    return m_bInit;
}

void Application::Deinit()
{
    Join();
    m_bInit = false;
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
    m_semaphore.Set();
}

