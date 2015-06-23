#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include "thread.h"
#include "../common/object_manager.h"
#include "../common/managers_container.h"

class ThreadManager : public ManagerCreator<ThreadManager>
{
protected:
	template<class Object> friend class ManagerCreator;
	ThreadManager();
	virtual ~ThreadManager();
public:
	void AddThread(Thread* thread);
	void RemoveThread(Thread* thread);

protected:
	template<typename TClass, typename TFunc, typename TObject> friend class ReferenceObject;
	bool CheckThread(const std::vector<Thread*>& threads, const Thread* thread);
	bool StopThread(const std::vector<Thread*>& threads, const Thread* thread);
protected:
	virtual void ManagerFunc();
	virtual void ManagerStop();
private:
	ObjectManager<Thread*> m_threads;
};

#endif/*THREAD_MANAGER_H*/