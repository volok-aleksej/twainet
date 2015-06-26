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
		: m_hostId(getCpuHash()), m_sessionId(0){}
	HostAddress(unsigned short hostId, const std::string& mac = "", unsigned short sessionId = 0)
		: m_hostId(hostId), m_mac(mac), m_sessionId(sessionId){}
	~HostAddress(){}

	bool operator == (const HostAddress& addr)
	{
		return addr.m_hostId == m_hostId;
	}

	void operator = (const HostAddress& addr)
	{
		m_hostId = addr.m_hostId;
		m_mac = addr.m_mac;
		m_sessionId = addr.m_sessionId;
	}
	
	unsigned short m_hostId;
	std::string m_mac;
	unsigned short m_sessionId;
};

class Application : public Singleton<Application>
{
protected:
	template<class Object> friend class Singleton;
	Application();
	~Application();

public:
	unsigned short GetOwnId();
	std::vector<HostAddress> GetIds();
	bool AddContact(const HostAddress& mac);
	bool RemoveContact(const HostAddress& mac);
	bool UpdateContact(const HostAddress& mac);
	HostAddress GetContact(unsigned short hostId);
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