#ifndef MANAGERS_CONTAINER_H
#define MANAGERS_CONTAINER_H

#include "..\common\thread_singleton.h"
#include "..\common\object_manager.h"

class IManager
{
public:
	virtual void ManagerFunc() = 0;
	virtual void ManagerStart() = 0;
	virtual void ManagerStop() = 0;
};

template<class Object>
class ManagerCreator : public Singleton<ManagerCreator<Object> >, public IManager
{
	static Object* object;
public:
	static Object& GetInstance()
	{
		static Object _object;
		object = &_object;
		ManagerCreator<Object>& object_ = Singleton<ManagerCreator<Object> >::GetInstance();
		return _object;
	}

protected:
	template<class Object> friend class Singleton;
	ManagerCreator()
	{
		if(object)
			ManagersContainer::GetInstance().AddManager(static_cast<IManager*>(object));
	}
	
	virtual ~ManagerCreator()
	{
		if(object)
		{
			ManagersContainer::GetInstance().RemoveManager(static_cast<IManager*>(object));
			object = 0;
		}
	}

protected:
	virtual void ManagerFunc(){};
	virtual void ManagerStart(){};
	virtual void ManagerStop(){};
};

template<class Object>
Object* ManagerCreator<Object>::object = 0;

class ManagersContainer : public ThreadSingleton<ManagersContainer>
{
protected:
	template<class Object> friend class Singleton;
	ManagersContainer();
	~ManagersContainer();

public:
	template<class Object> friend class ManagerCreator;
	void AddManager(IManager* manager);
	void RemoveManager(IManager* manager);

protected:
	template<typename TClass, typename TFunc> friend class Reference;
	void RunManager(const IManager* manager);
	bool CheckManager(const IManager* manager);
protected:
	virtual void ThreadFunc();
	virtual void OnStop();
	virtual void OnStart();
	virtual void Stop();
private:
	ObjectManager<IManager*> m_managers;
	bool m_isExit;
};

#endif/*MANAGERS_CONTAINER_H*/