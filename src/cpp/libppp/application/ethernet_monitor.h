#ifndef ETHERNET_MONITOR_H
#define ETHERNET_MONITOR_H

#include "net\pppoe_headers.h"
#include "net\parser_states.h"
#include "pcap.h"
#include <string>

class EthernetMonitor
{
public:
	EthernetMonitor(pcap_t *fp, const std::string& mac);
	virtual ~EthernetMonitor();

protected:
	template<class Container> friend void BasicState::OnPacket(Container container);
	void OnPacket(const PPPoEDContainer& container);

protected:
	friend class Application;
	void MonitorStart();
	bool MonitorFunc();
	void MonitorStop();
private:
	pcap_t *m_fp;
	std::string m_mac;
	int m_timeoutCount; //in second
	unsigned int m_currentClock;
	int m_currentPADISend;
};

#endif/*ETHERNET_MONITOR_H*/