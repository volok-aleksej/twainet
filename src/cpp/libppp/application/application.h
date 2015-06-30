#ifndef APPLICATION_H
#define APPLICATION_H

#include "thread_lib\common\thread_singleton.h"
#include "thread_lib\common\object_manager.h"

class EthernetMonitor;
class PPPConnection;

class PPPHost
{
public:
	PPPHost();
	PPPHost(const std::string& hostId, const std::string& mac = "", unsigned short sessionId = 0);
	~PPPHost();

	bool operator == (const PPPHost& addr) const;
	void operator = (const PPPHost& addr);
	
	std::string m_hostId;
	std::string m_mac;
	unsigned short m_sessionId;
	std::string m_hostName;
	std::string m_hostCookie;
	PPPConnection* m_connection;
};

class Application : public ThreadSingleton<Application>
{
protected:
	template<class Object> friend class Singleton;
	Application();
	~Application();

public:
	std::string GetOwnId();
	std::vector<PPPHost> GetIds();
	bool AddContact(const PPPHost& mac);
	bool RemoveContact(const PPPHost& mac);
	bool UpdateContact(const PPPHost& mac);
	PPPHost GetContact(const std::string& hostId);

protected:
	virtual void ThreadFunc();
	virtual void OnStop();
	virtual void OnStart();
	virtual void Stop();

	template<typename TClass, typename TFunc, typename TObject> friend class ReferenceObject;
	bool RemoveMonitor(const std::vector<EthernetMonitor*>& monitors, const EthernetMonitor* monitor);
	bool MonitorStep(const std::vector<EthernetMonitor*>& monitors, const EthernetMonitor* monitor);
	bool CheckPPPConnection(const PPPHost& id, const PPPHost& host);
	template<typename TClass, typename TFunc> friend class Reference;
	void MonitorStart(const EthernetMonitor* monitor);
	bool RemovePPPConnection(const PPPHost& host);

private:
	void DetectionEthernet();

private:
	ObjectManager<EthernetMonitor*> m_monitors;
	ObjectManager<PPPHost> m_contacts;
	static std::string m_computerId;
};

#endif/*APPLICATION_H*/