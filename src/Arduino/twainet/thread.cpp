#include <Arduino.h>
extern "C" {
#include <cont.h>
}
#include "thread_manager.h"

void Thread::sleep(unsigned long millisec)
{
    delay(millisec);
}

void Thread::ThreadFunc(Thread* thread)
{
    thread->ThreadFunc();
}

Thread::Thread(bool isDestroyable)
: m_threadId(0), m_destroyable(isDestroyable)
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

bool Thread::SuspendThread()
{
    if(IsRunning()) {
        ThreadManager::GetInstance().SwitchThread();
    }
    return false;
}

bool Thread::ResumeThread()
{
    if(IsSuspend()) {
        if(m_threadId) {
           return ets_post(m_threadId, m_threadId, 0);
        }
    }
    return false;
}

bool Thread::IsDestroyable()
{
    return m_destroyable;
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

bool Thread::IsSuspend()
{
    return g_threadDesks[m_threadId - THREAD_START_ID].m_state == ThreadDescription::WAITING;
}

void Thread::StopThread()
{
	if(!IsStopped())
	{
		g_threadDesks[m_threadId - THREAD_START_ID].m_state == ThreadDescription::STOP_PENDING;
        ThreadManager::GetInstance().RemoveThread(this);
	}
}
