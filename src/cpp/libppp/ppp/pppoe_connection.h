#ifndef PPPOED_CONNECTION_H
#define PPPOED_CONNECTION_H

#include <string>
#include "net\pppoe_headers.h"
#include "parser\net_packet.h"
#include "thread_lib\common\managers_container.h"

class EthernetMonitor;

class PPPoEConnection : public DynamicManager, public IConnection
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
	unsigned short GetSessionId() const;
	std::string GetHostName() const;
	std::string GetHostCookie() const;
	std::string GetMac() const;

protected:
	friend class EthernetMonitor;
	virtual bool IsConnectionPacket(IConnectionPacket* packet);
	virtual void ManagerFunc();
	virtual void ManagerStart();
	virtual void ManagerStop();

	void SendPPPoED(PPPoEDContainer container);

protected:
	template<typename TClass, typename TFunc> friend class Reference;
	template<typename TClass, typename TFunc, typename TObject> friend class ReferenceObject;
	bool DeleteContainer(const IConnectionPacket* pppoed);
	bool GetContainer(IConnectionPacket** const container, const IConnectionPacket* pppoed);

	void OnPacket(PPPoEDContainer* container);
	void OnContainer(PPPoEDContainer* container);
public:
	typedef ConnectionPacket<PPPoEDContainer, PPPoEConnection, &PPPoEConnection::OnPacket> PPPoEDPacket;
	typedef ConnectionPacket<PPPoEDContainer, PPPoEConnection, &PPPoEConnection::OnContainer> PPPoEDSelfPacket;

protected:
	EthernetMonitor* m_monitor;
	State m_statePPPoE;
	ObjectManager<IConnectionPacket*> m_containers;

private:
	std::string m_hostId;
	std::string m_mac;
	unsigned short m_sessionId;
	std::string m_hostName;
	std::string m_hostCookie;
};


#endif/*PPPOED_CONNECTION_H*/