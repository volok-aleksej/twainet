#ifndef APPLICATION_H
#define APPLICATION_H

#include "twainet.h"
#include "twainet/module/twainet_module.h"
#include "twainet/message/NotificationMessages.h"
#include "thread_lib/common/managers_container.h"

class Application : public ManagerCreator<Application>
{
protected:
	friend class ManagerCreator<Application>;
	Application();
	~Application();
	virtual void ManagerFunc();
	virtual void ManagerStop();
public:

	void Init(const Twainet::TwainetCallback& callback);
	TwainetModule* CreateModule(const Twainet::ModuleName& moduleName, int ipv);
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
};

#endif/*APPLICATION_H*/