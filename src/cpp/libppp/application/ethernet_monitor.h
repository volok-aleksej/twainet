#ifndef ETHERNET_MONITOR_H
#define ETHERNET_MONITOR_H

#include "thread_lib\thread\thread_impl.h"
#include "pcap.h"
#include <string>

class EthernetMonitor : public ThreadImpl
{
public:
	EthernetMonitor(pcap_t *fp, const std::string& mac);
	virtual ~EthernetMonitor();

protected:
	virtual void ThreadFunc();
	virtual void Stop();
private:
	pcap_t *m_fp;
	std::string m_mac;
};

#endif/*ETHERNET_MONITOR_H*/