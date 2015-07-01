#include "application.h"
#include "ethernet_monitor.h"
#include "common\ref.h"
#include "pcap.h"
#include "version.h"

pppoed_tag_vendor defaultVendor = {0xffc5ca87/*crc32 hash of TwainetPPP*/, {VER_NUMBER_MAJOR, VER_NUMBER_MINOR, VER_NUMBER_RELEASE, VER_NUMBER_BUILD}};
std::string getMAC(sockaddr_in* addr);
std::string CreateGUID();

std::string Application::m_computerId = CreateGUID();

Application::Application()
{
	DetectionEthernet();
	Start();
}

Application::~Application()
{
	Join();
}

bool Application::RemoveMonitor(const std::vector<EthernetMonitor*>& monitors, const EthernetMonitor* monitor)
{
	EthernetMonitor* monitor_ = const_cast<EthernetMonitor*>(monitor);
	const_cast<std::vector<EthernetMonitor*>&>(monitors).push_back(monitor_);
	return true;
}

bool Application::MonitorStep(const std::vector<EthernetMonitor*>& monitors, const EthernetMonitor* monitor)
{
	if(!const_cast<EthernetMonitor*>(monitor)->MonitorFunc())
	{
		EthernetMonitor* monitor_ = const_cast<EthernetMonitor*>(monitor);
		const_cast<std::vector<EthernetMonitor*>&>(monitors).push_back(monitor_);
		return true;
	}

	return false;
}

void Application::MonitorStart(const EthernetMonitor* monitor)
{
	const_cast<EthernetMonitor*>(monitor)->MonitorStart();
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

void Application::ThreadFunc()
{
	while(!IsStop())
	{
		std::vector<EthernetMonitor*> monitors;
		m_monitors.CheckObjects(Ref(this, &Application::MonitorStep, monitors));
		for(std::vector<EthernetMonitor*>::iterator it = monitors.begin();
			it != monitors.end(); it++)
		{
			(*it)->MonitorStop();
			delete *it;
		}
	}
}

void Application::OnStop()
{
	std::vector<EthernetMonitor*> monitors;
	m_monitors.CheckObjects(Ref(this, &Application::RemoveMonitor, monitors));
	for(std::vector<EthernetMonitor*>::iterator it = monitors.begin();
		it != monitors.end(); it++)
	{
		(*it)->MonitorStop();
		delete *it;
	}
}

void Application::OnStart()
{
	while(IsStop())
	{
		m_monitors.ProcessingObjects(Ref(this, &Application::MonitorStart));
	}
}

void Application::Stop()
{
	StopThread();
}

std::string Application::GetOwnId()
{
	return m_computerId;
}

std::vector<std::string> Application::GetIds()
{
	std::vector<std::string> ids;
//	m_contacts.ProcessingObjects(Ref(this, &Application::ListConnection, ids));
	return ids;
}
