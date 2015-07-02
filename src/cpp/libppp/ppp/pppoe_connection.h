#ifndef PPPOED_CONNECTION_H
#define PPPOED_CONNECTION_H

#include <string>
#include "net/pppoe_headers.h"
#include "thread_lib\common\managers_container.h"

class EthernetMonitor;

class PPPoEConnection : public DynamicManager
{
protected:
	PPPoEConnection(EthernetMonitor* monitor, const std::string& hostId);
public:
	virtual ~PPPoEConnection();

public:
	enum State
	{
		PADI = 0,
		PADO,
		PADR,
		PADS
	};

	std::string GetHostId() const;
	bool operator == (const PPPoEConnection& addr) const;
	void OnPacket(const PPPoEDContainer& pppoed);
	void OnPacket(const PPPoESContainer& pppoes);

protected:
	template<typename TClass, typename TFunc> friend class Reference;
	template<typename TClass, typename TFunc, typename TObject> friend class ReferenceObject;
	bool DeleteContainer(const PPPoEContainer* pppoed);
	bool GetContainer(PPPoEContainer** const container, const PPPoEContainer* pppoed);
protected:
	virtual void ManagerFunc();
	virtual void ManagerStart();
	virtual void ManagerStop();
protected:
	void SendPPPoED(PPPoEDContainer container);
	void OnContainer(PPPoEDContainer* container);
	void OnContainer(PPPoESContainer* container);
protected:
	std::string m_hostId;
	std::string m_mac;
	unsigned short m_sessionId;
	std::string m_hostName;
	std::string m_hostCookie;
	State m_statePPPoS;
	ObjectManager<PPPoEContainer*> m_containers;
	EthernetMonitor* m_monitor;
};

#endif/*PPPOED_CONNECTION_H*/