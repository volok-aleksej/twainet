#ifndef APPLICATION_H
#define APPLICATION_H

#include "thread_lib\common\thread_singleton.h"
#include "twainet.h"
#include "twainet\module\twainet_module.h"
#include "twainet\message\NotificationMessages.h"

class Application : public ThreadSingleton<Application>
{
protected:
	template<class Object> friend class Singleton;
	Application();
	~Application();
	virtual void ThreadFunc();
	virtual void OnStop();
	virtual void OnStart();
public:
	virtual void Stop();

	void Init(const Twainet::TwainetCallback& callback);
	TwainetModule* CreateModule(const Twainet::ModuleName& moduleName);
	void DeleteModule(TwainetModule* module);
protected:
	friend class TwainetModule;
	void AddNotifycationMessage(NotificationMessage* message);
private:
	CriticalSection m_csModules;
	std::vector<TwainetModule*> m_modules;
	CriticalSection m_csMessages;
	std::vector<NotificationMessage*> m_messages;
	Twainet::TwainetCallback m_callbacks;
	bool m_isExit;
};

#endif/*APPLICATION_H*/