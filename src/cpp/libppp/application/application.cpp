#include "application.h"
#include "ethernet_monitor.h"
#include "common\ref.h"
#include "pcap.h"
#include "version.h"

pppoed_tag_vendor defaultVendor = {0xffc5ca87/*crc32 hash of TwainetPPP*/, {VER_NUMBER_MAJOR, VER_NUMBER_MINOR, VER_NUMBER_RELEASE, VER_NUMBER_BUILD}};
std::string getMAC(sockaddr_in* addr);

Application::Application()
{
	DetectionEthernet();
}

Application::~Application()
{
	std::vector<EthernetMonitor*> monitors;
	m_monitors.CheckObjects(Ref(this, &Application::RemoveMonitor, monitors));
}

bool Application::RemoveMonitor(const std::vector<EthernetMonitor*>& monitors, const EthernetMonitor* monitor)
{
	EthernetMonitor* monitor_ = const_cast<EthernetMonitor*>(monitor);
	const_cast<std::vector<EthernetMonitor*>&>(monitors).push_back(monitor_);
	return true;
}

void Application::DetectionEthernet()
{
	pcap_if_t *alldevs;
    pcap_if_t *d;
	pcap_t *fp;
    int i=0;
    char errbuf[PCAP_ERRBUF_SIZE];
    
    if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
    {
		return;
    }
    
    for(d= alldevs; d != NULL; d= d->next)
    {
		sockaddr_in* addr;
		pcap_addr* paddr;
		for(paddr = d->addresses; paddr != 0; paddr = paddr->next)
		{
			addr = (sockaddr_in*)paddr->addr;
			if(addr->sin_family == AF_INET)
				break;
		}

		if ((fp = pcap_open_live(d->name, 65536, 1, 1000, errbuf)) != NULL)
		{
			EthernetMonitor* monitor = new EthernetMonitor(fp, getMAC(addr));
			m_monitors.AddObject(monitor);
		}
	}
	
    pcap_freealldevs(alldevs);
}

unsigned short Application::GetOwnId()
{
	return getCpuHash();
}

bool Application::AddContact(const HostAddress& mac)
{
	HostAddress addr;
	if(!m_contacts.GetObject(mac, &addr))
	{
		return m_contacts.AddObject(mac);
	}
	else if(addr.m_mac != mac.m_mac)
	{
		return m_contacts.UpdateObject(mac);
	}
	return false;	
}

bool Application::UpdateContact(const HostAddress& mac)
{
	return m_contacts.UpdateObject(mac);
}

HostAddress Application::GetContact(unsigned short hostId)
{
	HostAddress addr(hostId);
	m_contacts.GetObject(addr, &addr);
	return addr;
}

bool Application::RemoveContact(const HostAddress& mac)
{
	return m_contacts.RemoveObject(mac);
}

std::vector<HostAddress> Application::GetIds()
{
	return m_contacts.GetObjectList();
}
