#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include "thread.h"
#include "include/singleton.h"
#include "std/list.hpp"

class ThreadManager : public Singleton<ThreadManager>
{
protected:
	friend class Singleton<ThreadManager>;
	ThreadManager();
	virtual ~ThreadManager();
public:
	void AddThread(Thread* thread);
	void RemoveThread(Thread* thread);
    
    void CheckThreads();
private:
	twnstd::list<Thread*> m_threads;
};

#endif/*THREAD_MANAGER_H*/