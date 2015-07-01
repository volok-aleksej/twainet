#ifndef APPLICATION_H
#define APPLICATION_H

#include "thread_lib\common\thread_singleton.h"
#include "thread_lib\common\object_manager.h"

class EthernetMonitor;

class Application : public ThreadSingleton<Application>
{
protected:
	template<class Object> friend class Singleton;
	Application();
	~Application();

public:
	std::string GetOwnId();
	std::vector<std::string> GetIds();

protected:
	virtual void ThreadFunc();
	virtual void OnStop();
	virtual void OnStart();
	virtual void Stop();

protected:
	template<typename TClass, typename TFunc, typename TObject> friend class ReferenceObject;
	template<typename TClass, typename TFunc> friend class Reference;
	bool RemoveMonitor(const std::vector<EthernetMonitor*>& monitors, const EthernetMonitor* monitor);
	bool MonitorStep(const std::vector<EthernetMonitor*>& monitors, const EthernetMonitor* monitor);
	void MonitorStart(const EthernetMonitor* monitor);

private:
	void DetectionEthernet();

private:
	ObjectManager<EthernetMonitor*> m_monitors;
	static std::string m_computerId;
};

#endif/*APPLICATION_H*/