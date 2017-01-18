#include <Arduino.h>

#include "thread_manager.h"

void Thread::sleep(unsigned long millisec)
{
    delay(millisec);
}

Thread::Thread()
: m_threadId(0)
{
}

Thread::~Thread()
{
}

bool Thread::StartThread()
{
    ThreadManager::GetInstance().AddThread(this);
    if(m_threadId) {
        ets_post(m_threadId, m_threadId, 0);
    }
}

bool Thread::IsStopped() const
{
	return g_threadDesks[m_threadId - THREAD_START_ID].m_state == ThreadDescription::STOPPED;
}

bool Thread::IsStop()
{
    return g_threadDesks[m_threadId - THREAD_START_ID].m_state == ThreadDescription::STOP_PENDING;
}

bool Thread::IsRunning()
{
    return g_threadDesks[m_threadId - THREAD_START_ID].m_state == ThreadDescription::RUNNING;
}


void Thread::StopThread()
{
	if(!IsStopped())
	{
		g_threadDesks[m_threadId - THREAD_START_ID].m_state == ThreadDescription::STOP_PENDING;
        ThreadManager::GetInstance().RemoveThread(this);
	}
}
