#ifndef PPPOED_CONNECTION_H
#define PPPOED_CONNECTION_H

#include <string>
#include "net/pppoe_headers.h"
#include "thread_lib\common\managers_container.h"

class EthernetMonitor;

class PPPoEDConnection : public DynamicManager
{
public:
	enum State
	{
		PADI = 0,
		PADO,
		PADR,
		PADS
	};

public:
	PPPoEDConnection(EthernetMonitor* monitor, const std::string& hostId);
	virtual ~PPPoEDConnection();

	std::string GetHostId() const;
	bool operator == (const PPPoEDConnection& addr) const;
	void OnPacket(const PPPoEDContainer& pppoed);

protected:
	template<typename TClass, typename TFunc> friend class Reference;
	template<typename TClass, typename TFunc, typename TObject> friend class ReferenceObject;
	bool DeleteContainer(const PPPoEDContainer* pppoed);
	bool GetContainer(PPPoEDContainer** const container, const PPPoEDContainer* pppoed);
protected:
	void ManagerFunc();
	void ManagerStart();
	void ManagerStop();
protected:
	void SendPPPoED(PPPoEDContainer container);
protected:
	std::string m_hostId;
	std::string m_mac;
	unsigned short m_sessionId;
	std::string m_hostName;
	std::string m_hostCookie;
	State m_state;
	ObjectManager<PPPoEDContainer*> m_containers;
	EthernetMonitor* m_monitor;
};

#endif/*PPPOED_CONNECTION_H*/