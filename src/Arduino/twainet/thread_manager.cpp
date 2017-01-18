#include "thread_manager.h"
#include <malloc.h>
#include <string.h>

unsigned int g_threadCount = 0;

static uint32_t g_current_threadId = 0;
ThreadDescription g_threadDesks[THREAD_MAX];

void thread_wrapper()
{
    ThreadDescription& desk = g_threadDesks[g_current_threadId - THREAD_START_ID];
    desk.m_state = ThreadDescription::RUNNING;
    ThreadManager::GetInstance().RunThreadFunc(desk.m_thread);
}

static void thread_func(os_event_t *events)
{
    ThreadDescription& desk = g_threadDesks[events->sig - THREAD_START_ID];
    g_current_threadId = events->sig;
    if(desk.m_state == ThreadDescription::CREATED ||
              desk.m_state == ThreadDescription::WAITING) {
        cont_run(&desk.m_cont, &thread_wrapper);
        if (cont_check(&desk.m_cont) != 0) {
            panic();
        }
    }
    
    desk.m_state = ThreadDescription::STOPPED;
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
            g_threadDesks[i].m_thread->m_threadId = g_threadDesks[i].m_id;
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
            g_threadDesks[i].m_thread->Stop();
            if(g_threadDesks[i].m_id != g_current_threadId) {
                delete g_threadDesks[i].m_thread;
                g_threadDesks[i].m_state = ThreadDescription::ABSENT;
            } else {
                g_threadDesks[i].m_state = ThreadDescription::STOP_PENDING;
            }
            break;
        }
    }
}

unsigned int ThreadManager::GetCurrentThreadId()
{
    return g_current_threadId;
}

unsigned int ThreadManager::GetNextSuspendThreadId()
{
    if(g_current_threadId) {
        for(uint8_t i = g_current_threadId - THREAD_START_ID; i < THREAD_MAX; i++) {
            if(g_threadDesks[i].m_state == ThreadDescription::WAITING) {
                return g_threadDesks[i].m_id;
            }
        }
        
        for(uint8_t i = 0; i < g_current_threadId - THREAD_START_ID; i++) {
            if(g_threadDesks[i].m_state == ThreadDescription::WAITING) {
                return g_threadDesks[i].m_id;
            }
        }
    } else {
        for(uint8_t i = 0; i < THREAD_MAX; i++) {
            if(g_threadDesks[i].m_state == ThreadDescription::WAITING) {
                return g_threadDesks[i].m_id;
            }
        }
    }
    
    return 0;
}
    
void ThreadManager::RunThreadFunc(Thread* thread)
{
    thread->ThreadFunc();
}

extern "C" void esp_schedule();

void ThreadManager::SwitchThread()
{
    unsigned int id = ThreadManager::GetInstance().GetNextSuspendThreadId();
    if(id) {
        ets_post(id, id, 0);
    } else {
        esp_schedule();
    }
}

void ThreadManager::CheckThreads()
{
    for(uint8_t i = 0; i < THREAD_MAX; i++) {
        if(g_threadDesks[i].m_state == ThreadDescription::STOPPED) {
            delete g_threadDesks[i].m_thread;
            g_threadDesks[i].m_state = ThreadDescription::ABSENT;
        }
    }
}
