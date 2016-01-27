#ifndef MANAGERS_CONTAINER_H
#define MANAGERS_CONTAINER_H

#include "include/object_manager.h"
#include "../common/thread_singleton.h"

class IManager
{
private:
	friend class DynamicManager;
	friend class StaticManager;
	IManager(){}

public:
	virtual ~IManager(){}

	virtual void ManagerFunc() = 0;
	virtual void ManagerStart() = 0;
	virtual void ManagerStop() = 0;
	virtual bool IsStop() = 0;
};

class DynamicManager : public IManager
{
public:
	DynamicManager();
	virtual ~DynamicManager();
	virtual bool IsStop();

	void Stop();
private:
	bool m_isStop;
};

template<class Object> class ManagerCreator;

class ManagersContainer : public ThreadSingleton<ManagersContainer>
{
protected:
	friend class Singleton<ManagersContainer>;
	ManagersContainer();
	~ManagersContainer();

public:
	friend class ManagerCreator<ManagersContainer>;
	void AddManager(IManager* manager);
	void RemoveManager(IManager* manager);

protected:
	template<typename TClass, typename TFunc, typename TObject, typename TReturn> friend class ReferenceObject;
	bool RunManager(const std::vector<IManager*>& managers, const IManager* manager);
	bool CheckManager(const std::vector<IManager*>& managers, const IManager* manager);
protected:
	virtual void ThreadFunc();
	virtual void OnStop();
	virtual void OnStart();
	virtual void Stop();
private:
	ObjectManager<IManager*> m_managers;
	bool m_isExit;
};

template<class Object>
class ManagerCreator : public Singleton<ManagerCreator<Object> >, public DynamicManager
{
	static Object* object;
public:
	static Object& GetInstance()
	{
		if(!object)
			object = new Object;
		ManagerCreator<Object>& object_ = Singleton<ManagerCreator<Object> >::GetInstance();
		return *object;
	}

	virtual bool IsDelete()	
	{
		return true;
	}

	virtual bool IsStop()
	{
		return false;
	}

protected:
	friend class Singleton<ManagerCreator>;
	ManagerCreator()
	{
		if(object)
			ManagersContainer::GetInstance().AddManager(static_cast<IManager*>(object));
	}
	
	virtual ~ManagerCreator()
	{
		if(object)
			object = 0;
	}

protected:
	virtual void ManagerFunc(){};
	virtual void ManagerStart(){};
	virtual void ManagerStop(){};
};

template<class Object>
Object* ManagerCreator<Object>::object = 0;

#endif/*MANAGERS_CONTAINER_H*/