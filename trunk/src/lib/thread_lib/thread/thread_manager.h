#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include "thread.h"
#include "../common/object_manager.h"
#include "../common/thread_singleton.h"

class ThreadManager : public ThreadSingleton<ThreadManager>
{
protected:
	template<class Object> friend class Singleton;
	ThreadManager();
	~ThreadManager();
public:
	void AddThread(Thread* thread);
	void RemoveThread(Thread* thread);

protected:
	template<typename TClass, typename TFunc, typename TObject> friend class ReferenceObject;
	bool CheckThread(const std::vector<Thread*>& threads, const Thread* thread);
	bool StopThread(const std::vector<Thread*>& threads, const Thread* thread);
protected:
	virtual void ThreadFunc();
	virtual void OnStop();
	virtual void OnStart();
	virtual void Stop();
private:
	ObjectManager<Thread*> m_threads;
	bool m_isExit;
};

#endif/*THREAD_MANAGER_H*/