#include "thread_manager.h"
#include <malloc.h>
#include <string.h>

unsigned int g_threadCount = 0;

ThreadManager::ThreadManager()
{
}

ThreadManager::~ThreadManager()
{
}

void ThreadManager::AddThread(Thread* thread)
{
    m_threads.insert(m_threads.end(), thread);
}

void ThreadManager::RemoveThread(Thread* thread)
{
    for(twnstd::list<Thread*>::iterator it = m_threads.begin();
        it != m_threads.end(); ++it)
    {
        if(*it == thread)
        {
            (*it)->StopThread();
            delete *it;
            m_threads.erase(it);
            break;
        }
    }
}

void ThreadManager::CheckThreads()
{
    for(twnstd::list<Thread*>::iterator it = m_threads.begin(); it != m_threads.end(); ++it)
    {
        if((*it)->IsRunning()) {
            (*it)->ThreadFunc();
        } else if((*it)->IsStopped()) {
            delete *it;
            it = m_threads.erase(it);
        }
    }
}
