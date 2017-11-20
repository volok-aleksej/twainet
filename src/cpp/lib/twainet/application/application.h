#ifndef APPLICATION_H
#define APPLICATION_H

#include "include/twainet.h"
#include "twainet/module/twainet_module.h"
#include "twainet/message/NotificationMessages.h"
#include "thread_lib/common/managers_container.h"

class Application : public ThreadSingleton<Application>
{
protected:
	friend class Singleton<Application>;
	Application();
	~Application();
	virtual void ThreadFunc();
	virtual void OnStop();
	virtual void OnStart();
	virtual void Stop();
public:

	void Init(const Twainet::TwainetCallback& callback);
    	bool IsInited();
    	void Deinit();
	TwainetModule* CreateModule(const char* moduleName, int ipv);
	void DeleteModule(TwainetModule* module);
protected:
	friend class TwainetModule;
	void AddNotifycationMessage(NotificationMessage* message);
private:
	CriticalSection m_csModules;
	std::vector<TwainetModule*> m_modules;
	CriticalSection m_csMessages;
	std::vector<NotificationMessage*> m_messages;
    	Semaphore m_semaphore;
	Twainet::TwainetCallback m_callbacks;
    	bool m_bInit;
};

#endif/*APPLICATION_H*/
