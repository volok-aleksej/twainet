#ifndef PPPOED_CONNECTION_H
#define PPPOED_CONNECTION_H

#include <string>
#include "net\pppoe_headers.h"
#include "parser\net_packet.h"
#include "thread_lib\common\managers_container.h"

class EthernetMonitor;
class PPPoEConnection;

typedef ConnectionPacket<PPPoEDContainer, PPPoEConnection> PPPoEDPacket;

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
protected:
	virtual void ManagerFunc();
	virtual void ManagerStart();
	virtual void ManagerStop();
protected:
	void SendPPPoED(PPPoEDContainer container);
	void OnContainer(PPPoEDContainer* container);

protected:
	std::string m_hostId;
	std::string m_mac;
	unsigned short m_sessionId;
	std::string m_hostName;
	std::string m_hostCookie;
	State m_statePPPoS;
	EthernetMonitor* m_monitor;

private:
	ObjectManager<PPPoEDContainer*> m_containers;

	template<typename TClass, typename TFunc> friend class Reference;
	template<typename TClass, typename TFunc, typename TObject> friend class ReferenceObject;
	bool DeleteContainer(const PPPoEDContainer* pppoed);
	bool GetContainer(PPPoEDContainer** const container, const PPPoEDContainer* pppoed);

	template<typename Packet, typename Connection> friend class ConnectionPacket;
	void OnPacket(PPPoEDContainer* container);
};

#endif/*PPPOED_CONNECTION_H*/