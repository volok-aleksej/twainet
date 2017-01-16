#include <Arduino.h>

#include "thread_manager.h"

void Thread::sleep(unsigned long millisec)
{
    delay(millisec);
}

Thread::Thread()
: m_threadId(0)
, m_state(THREAD_CREATED)
{
}

Thread::~Thread()
{
}

bool Thread::StartThread()
{
    if(m_state != THREAD_STOPPED)
    {
        return false;
    }
    m_state = THREAD_START_PENDING;
    m_threadId = g_threadCounter + 1;
    g_threadCounter++;
    OnStart();
    m_state = Thread::THREAD_RUNNING;
	return true;
}

bool Thread::IsStopped() const
{
	return m_state == THREAD_STOPPED;
}

bool Thread::IsStop()
{
	return m_state == THREAD_STOP_PENDING;
}

bool Thread::IsRunning()
{
	return m_state == THREAD_RUNNING;
}


void Thread::StopThread()
{
	if(m_state != THREAD_STOPPED)
	{
		m_state = THREAD_STOP_PENDING;
        Stop();
        m_state = THREAD_STOPPED;
	}
}

ThreadImpl::ThreadImpl()
{
}

ThreadImpl::~ThreadImpl()
{
}

void ThreadImpl::OnStop()
{
    ThreadManager::GetInstance().RemoveThread(this);
}

void ThreadImpl::OnStart()
{
    ThreadManager::GetInstance().AddThread(this);
}
