#ifndef MANAGERS_CONTAINER_H
#define MANAGERS_CONTAINER_H

#include "std/list.hpp"
#include "include/thread_singleton.h"

class IManager
{
public:
	IManager(){}
	virtual ~IManager(){}

	virtual void ManagerFunc() = 0;
	virtual void ManagerStart() = 0;
	virtual void ManagerStop() = 0;
    virtual bool IsStop() = 0;
    virtual bool IsDestroyable() = 0;
};

class ManagersContainer : public ThreadSingleton<ManagersContainer>
{
protected:
	friend class Singleton<ManagersContainer>;
	ManagersContainer();
	~ManagersContainer();

public:
	void AddManager(IManager* manager);
	void RemoveManager(IManager* manager);

protected:
	virtual void ThreadFunc();
	virtual void OnStop();
	virtual void OnStart();
	virtual void Stop();
private:
	twnstd::list<IManager*> m_managers;
	bool m_isExit;
};

#endif/*MANAGERS_CONTAINER_H*/