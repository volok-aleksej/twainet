#ifndef ETHERNET_MONITOR_H
#define ETHERNET_MONITOR_H

#include "thread_lib\common\managers_container.h"
#include "pcap.h"

class EthernetMonitor : public IManager
{
public:
	EthernetMonitor(pcap_t *fp, const std::string& mac);
	virtual ~EthernetMonitor();
protected:
	void ManagerFunc();
	void ManagerStart();
	void ManagerStop();
private:
	pcap_t *m_fp;
	std::string m_mac;
};

#endif/*ETHERNET_MONITOR_H*/