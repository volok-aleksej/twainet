#include "thread_manager.h"
#include <malloc.h>
#include <string.h>
#include <Arduino.h>

unsigned int g_threadCount = 0;

static uint32_t g_current_threadId = 0;
ThreadDescription g_threadDesks[THREAD_MAX];

void thread_wrapper()
{
    ThreadDescription& desk = g_threadDesks[g_current_threadId - THREAD_START_ID];
    Thread::ThreadFunc(desk.m_thread);
    desk.m_state = ThreadDescription::STOPPED;
}

static void thread_func(os_event_t *events)
{
    ThreadDescription& desk = g_threadDesks[events->sig - THREAD_START_ID];
    g_current_threadId = events->sig;
    if(desk.m_state == ThreadDescription::CREATED ||
              desk.m_state == ThreadDescription::WAITING) {
        desk.m_state = ThreadDescription::RUNNING;
        cont_run(&desk.m_cont, &thread_wrapper);
        if (cont_check(&desk.m_cont) != 0) {
            panic();
        }
    }
}

extern "C" void esp_schedule();
extern "C" void esp_yield();

ThreadManager::ThreadManager()
{
    for(uint8_t i = 0; i < THREAD_MAX; i++) {
        g_threadDesks[i].m_id = THREAD_START_ID + i;
        g_threadDesks[i].m_thread = 0;
        g_threadDesks[i].m_state = ThreadDescription::ABSENT;
        cont_init(&g_threadDesks[i].m_cont);
        if(!ets_task(thread_func, g_threadDesks[i].m_id, g_threadDesks[i].m_loop_queue, QUEUE_SIZE)) {
            panic();
        }
    }
    
    ManagersContainer::GetInstance().AddManager(this);
}

ThreadManager::~ThreadManager()
{
    ManagersContainer::GetInstance().RemoveManager(this);
}

void ThreadManager::AddThread(Thread* thread)
{
    Serial.println("AddThread");
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

void ThreadManager::SuspendThread(unsigned int id)
{
    g_threadDesks[id - THREAD_START_ID].m_state = ThreadDescription::SUSPENDED;
    if(g_current_threadId == id) {
        unsigned int nextid = ThreadManager::GetInstance().GetNextSuspendThreadId();
        if(nextid) {
            ets_post(nextid, nextid, 0);
        } else {
            g_current_threadId = 0;
            esp_schedule();
        }

        if(id){
            if(cont_can_yield(&g_threadDesks[id - THREAD_START_ID].m_cont))
                cont_yield(&g_threadDesks[id - THREAD_START_ID].m_cont);
        } else {
            esp_yield();
        }
    }
}

void ThreadManager::ResumeThread(unsigned int id)
{
    unsigned int curId = g_current_threadId;
    if(id) {
        ets_post(id, id, 0);
    } else {
        g_current_threadId = 0;
        esp_schedule();
    }
    
    if(curId){
        if(cont_can_yield(&g_threadDesks[curId - THREAD_START_ID].m_cont))
            cont_yield(&g_threadDesks[curId - THREAD_START_ID].m_cont);
    } else {
        esp_yield();
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
    } else {
        for(uint8_t i = 0; i < THREAD_MAX; i++) {
            if(g_threadDesks[i].m_state == ThreadDescription::WAITING) {
                return g_threadDesks[i].m_id;
            }
        }
    }
    
    return 0;
}

void ThreadManager::SwitchThread()
{
    unsigned int id = ThreadManager::GetInstance().GetNextSuspendThreadId();
    unsigned int curId = g_current_threadId;
    if(id) {
        ets_post(id, id, 0);
    } else {
        g_current_threadId = 0;
        esp_schedule();
    }
    
    if(curId && g_threadDesks[curId - THREAD_START_ID].m_state == ThreadDescription::RUNNING){
        g_threadDesks[curId - THREAD_START_ID].m_state = ThreadDescription::WAITING;
        if(cont_can_yield(&g_threadDesks[curId - THREAD_START_ID].m_cont))
            cont_yield(&g_threadDesks[curId - THREAD_START_ID].m_cont);
    } else {
        esp_yield();
    }
}

void ThreadManager::ManagerFunc()
{
    for(uint8_t i = 0; i < THREAD_MAX; i++) {
        if(g_threadDesks[i].m_state == ThreadDescription::STOPPED) {
            if(g_threadDesks[i].m_thread->IsDestroyable())
                delete g_threadDesks[i].m_thread;
            g_threadDesks[i].m_thread = 0;
            g_threadDesks[i].m_state = ThreadDescription::ABSENT;
        }
    }
    
    SwitchThread();
}
