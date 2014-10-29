#include "application.h"
#include "memory.h"

Application::Application()
{
	memset(&m_callbacks, 0, sizeof(m_callbacks));
}

Application::~Application()
{
}

void Application::Init(const Twainet::TwainetCallback& callback)
{
	memcpy(&m_callbacks, &callback, sizeof(m_callbacks));
}
