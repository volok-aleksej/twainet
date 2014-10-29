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
