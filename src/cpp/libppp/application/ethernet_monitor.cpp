#include "ethernet_monitor.h"

EthernetMonitor::EthernetMonitor(pcap_t *fp, const std::string& mac)
	: m_fp(fp), m_mac(mac)
{
	ManagersContainer::GetInstance().AddManager(static_cast<IManager*>(this));
}

EthernetMonitor::~EthernetMonitor()
{
	ManagersContainer::GetInstance().RemoveManager(static_cast<IManager*>(this));
}

void EthernetMonitor::ManagerStart()
{
	//TODO(): create PADI request
}

void EthernetMonitor::ManagerFunc()
{
}