#include "ethernet_monitor.h"
#include "net\inet_headers.h"

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
	PPPoEDContainer container(m_mac, "ff:ff:ff:ff:ff:ff", PPPOE_PADI);
	container.m_tags.insert(std::make_pair(PPPOED_VS, PPPOED_VENDOR));
	int len;
	container.deserialize(0, len);
	unsigned char* data = new unsigned char[len];
	if(container.deserialize((char*)data, len))
	{
		pcap_sendpacket(m_fp, data, len);
	}

	delete data;
}

void EthernetMonitor::ManagerFunc()
{
}

void EthernetMonitor::ManagerStop()
{
	//TODO(): create PADT request
}