#include "application.h"
#include "ethernet_monitor.h"
#include "common\ref.h"
#include "pcap.h"

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

char* Application::GetOwnId()
{
	return "test";
}
