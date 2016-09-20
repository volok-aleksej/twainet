#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include "thread.h"
#include "singleton.h"
#include "list.hpp"

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