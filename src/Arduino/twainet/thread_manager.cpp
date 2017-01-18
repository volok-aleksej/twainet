#include "thread_manager.h"
#include <malloc.h>
#include <string.h>

unsigned int g_threadCount = 0;

static uint32_t g_current_threadId = 0;
static ThreadDescription g_threadDesks[THREAD_MAX];

void thread_wrapper()
{
    ThreadDescription& desk = g_threadDesks[g_current_threadId];
    ThreadManager::GetInstance().StartThread(desk.m_thread);
}

static void thread_func(os_event_t *events)
{
    ThreadDescription& desk = g_threadDesks[events->sig];
    g_current_threadId = events->sig;
    desk.m_state = ThreadDescription::RUNNING;
    cont_run(&desk.m_cont, &thread_wrapper);
    if (cont_check(&desk.m_cont) != 0) {
        panic();
    }
}

ThreadManager::ThreadManager()
{
    for(uint8_t i = 0; i < THREAD_MAX; i++) {
        g_threadDesks[i].m_id = THREAD_START_ID + i;
        g_threadDesks[i].m_thread = 0;
        g_threadDesks[i].m_state = ThreadDescription::ABSENT;
        if(!ets_task(thread_func, g_threadDesks[i].m_id, g_threadDesks[i].m_loop_queue, QUEUE_SIZE)) {
            panic();
        }
    }
}

ThreadManager::~ThreadManager()
{
}

void ThreadManager::AddThread(Thread* thread)
{
    for(uint8_t i = 0; i < THREAD_MAX; i++) {
        if(g_threadDesks[i].m_state == ThreadDescription::ABSENT) {
            g_threadDesks[i].m_thread = thread;
            g_threadDesks[i].m_state = ThreadDescription::CREATED;
            return;
        }
    }
    
    panic();
}

void ThreadManager::RemoveThread(Thread* thread)
{
    for(uint8_t i = 0; i < THREAD_MAX; i++) {
        if(g_threadDesks[i].m_thread == thread) {
            g_threadDesks[i].m_thread->StopThread();
            delete g_threadDesks[i].m_thread;
            g_threadDesks[i].m_state = ThreadDescription::ABSENT;
            break;
        }
    }
}

void ThreadManager::StartThread(Thread* thread)
{
    thread->ThreadFunc();
}

void ThreadManager::CheckThreads()
{/*
    for(twnstd::list<Thread*>::iterator it = m_threads.begin(); it != m_threads.end(); ++it)
    {
        if((*it)->IsRunning()) {
            (*it)->ThreadFunc();
        } else if((*it)->IsStopped()) {
            delete *it;
            it = m_threads.erase(it);
        }
    }*/
}
