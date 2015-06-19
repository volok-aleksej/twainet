#ifndef APPLICATION_H
#define APPLICATION_H

#include "common\singleton.h"
#include "thread_lib\common\object_manager.h"

class EthernetMonitor;

class Application : public Singleton<Application>
{
protected:
	template<class Object> friend class Singleton;
	Application();
	~Application();
public:
	char* GetOwnId();
protected:
	template<typename TClass, typename TFunc, typename TObject> friend class ReferenceObject;
	bool RemoveMonitor(const std::vector<EthernetMonitor*>& monitors, const EthernetMonitor* monitor);
private:
	void DetectionEthernet();
private:
	ObjectManager<EthernetMonitor*> m_monitors;
};

#endif/*APPLICATION_H*/