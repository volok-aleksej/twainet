#ifndef ETHERNET_MONITOR_H
#define ETHERNET_MONITOR_H

#include "thread_lib\common\managers_container.h"
#include "net\pppoe_headers.h"
#include "net\parser_states.h"
#include "pcap.h"
#include <string>

class EthernetMonitor : public DynamicManager
{
public:
	EthernetMonitor(pcap_t *fp, const std::string& mac);
	virtual ~EthernetMonitor();

protected:
	template<class Container> friend void BasicState::OnPacket(Container container);
	void OnPacket(const PPPoEDContainer& container);

protected:
	virtual void ManagerStart();
	virtual void ManagerFunc();
	virtual void ManagerStop();
private:
	pcap_t *m_fp;
	std::string m_mac;
	int m_timeoutCount; //in second
	int m_currentPADISend;
};

#endif/*ETHERNET_MONITOR_H*/