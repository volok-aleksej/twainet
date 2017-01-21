#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include "thread.h"
#include "managers_container.h"
#include "include/singleton.h"
#include "std/list.hpp"
#include "std/vector.hpp"

extern "C" {
#include <ets_sys.h>
#include <os_type.h>
#include <cont.h>
#include <debug.h>
}
    
#define THREAD_START_ID 2
#define QUEUE_SIZE      1
#define THREAD_MAX      3

struct ThreadDescription
{
    cont_t m_cont __attribute__ ((aligned (16)));
    os_event_t m_loop_queue[QUEUE_SIZE];
    unsigned int m_id;
    Thread* m_thread;
    enum {
        ABSENT,
        CREATED,
        RUNNING,
        SUSPENDED,
        WAITING,
        STOPPED,
        STOP_PENDING
    } m_state;
};

extern ThreadDescription g_threadDesks[THREAD_MAX];

class ThreadManager : public Singleton<ThreadManager>, public IManager
{
protected:
	friend class Singleton<ThreadManager>;
	ThreadManager();
	virtual ~ThreadManager();
public:
	void AddThread(Thread* thread);
	void RemoveThread(Thread* thread);
    void SuspendThread(unsigned int id);
    void ResumeThread(unsigned int id);
    void SwitchThread();
    unsigned int GetCurrentThreadId();
    
protected:
    virtual void ManagerFunc();
    virtual void ManagerStart(){}
    virtual void ManagerStop(){}
    virtual bool IsStop() { return false; }
    virtual bool IsDestroyable() { return false; }
protected:
    unsigned int GetNextSuspendThreadId();
};

#endif/*THREAD_MANAGER_H*/