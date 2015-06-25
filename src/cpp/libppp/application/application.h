#ifndef APPLICATION_H
#define APPLICATION_H

#include "common\singleton.h"
#include "thread_lib\common\object_manager.h"

class EthernetMonitor;
unsigned short getCpuHash();

class HostAddress
{
public:
	HostAddress()
		: m_hostId(getCpuHash()){}
	HostAddress(unsigned short hostId, const std::string& mac)
		: m_hostId(hostId), m_mac(mac){}
	~HostAddress(){}

	bool operator == (const HostAddress& addr)
	{
		return addr.m_hostId == m_hostId;
	}

	void operator = (const HostAddress& addr)
	{
		m_hostId = addr.m_hostId;
		m_mac = addr.m_mac;
	}
	
	unsigned short m_hostId;
	std::string m_mac;
};

class Application : public Singleton<Application>
{
protected:
	template<class Object> friend class Singleton;
	Application();
	~Application();

public:
	char* GetOwnId();
	void AddContact(const HostAddress& mac);
	void RemoveContact(const HostAddress& mac);
protected:
	template<typename TClass, typename TFunc, typename TObject> friend class ReferenceObject;
	bool RemoveMonitor(const std::vector<EthernetMonitor*>& monitors, const EthernetMonitor* monitor);
private:
	void DetectionEthernet();
private:
	ObjectManager<EthernetMonitor*> m_monitors;
	ObjectManager<HostAddress> m_contacts;
};

#endif/*APPLICATION_H*/