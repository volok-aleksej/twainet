#include "ethernet_monitor.h"
#include "net\inet_headers.h"
#include "net\parser_states.h"
#include "application\application.h"

EthernetMonitor::EthernetMonitor(pcap_t *fp, const std::string& mac)
	: m_fp(fp), m_mac(mac)
{
	Start();
}

EthernetMonitor::~EthernetMonitor()
{
	Join();
}

void EthernetMonitor::ThreadFunc()
{
	int len = 0;
	unsigned char* data = 0;
	//Send PADI
	PPPoEDContainer padi(m_mac, "ff:ff:ff:ff:ff:ff", PPPOE_PADI);
	padi.deserialize(0, len);
	data = new unsigned char[len];
	if(padi.deserialize((char*)data, len))
	{
		pcap_sendpacket(m_fp, data, len);
	}
	delete data;
	data = 0;

	bpf_program fcode;
	int res = pcap_compile(m_fp, &fcode, "pppoed or pppoes", 1, 0xffffff);
    res = pcap_setfilter(m_fp, &fcode);

	while(!IsStop())
	{
		pcap_pkthdr* header;
		const unsigned char* data;
		if (pcap_next_ex(m_fp, &header, &data) < 0 ||
			!data)
		{
			break;
		}

		BasicState state(this);
		BasicState *nextState = &state;
		while(nextState)
		{
			nextState = nextState->NextState((char*)data, header->len);
		}
	}
}

void EthernetMonitor::Stop()
{
	int len = 0;
	PPPoEDContainer padt(m_mac, ETHER_BROADCAST, PPPOE_PADT);
	padt.deserialize(0, len);
	unsigned char *data = new unsigned char[len];
	if(padt.deserialize((char*)data, len))
	{
		pcap_sendpacket(m_fp, data, len);
	}
	delete data;
	data = 0;

	pcap_close(m_fp);
}

void EthernetMonitor::OnPacket(const PPPoEDContainer& container)
{
	if(const_cast<PPPoEDContainer&>(container).m_tags[PPPOED_VS] != PPPOED_DEFAULT_VENDOR)
		return;

	if (EtherNetContainer::MacToString((char*)container.m_ethHeader.ether_dhost) == ETHER_BROADCAST &&
		EtherNetContainer::MacToString((char*)container.m_ethHeader.ether_shost) != m_mac)
	{
		HostAddress addr((unsigned short)const_cast<PPPoEDContainer&>(container).m_tags[PPPOED_HU].c_str(),
						EtherNetContainer::MacToString((char*)container.m_ethHeader.ether_shost));

		switch(container.m_pppoeHeader.code)
		{
			case PPPOE_PADI:
			{
				int len = 0;
				PPPoEDContainer pado(m_mac, EtherNetContainer::MacToString((char*)container.m_ethHeader.ether_shost), PPPOE_PADO);
				pado.deserialize(0, len);
				unsigned char *data = new unsigned char[len];
				if(pado.deserialize((char*)data, len))
				{
					pcap_sendpacket(m_fp, data, len);
				}
				delete data;
				data = 0;

				Application::GetInstance().AddContact(addr);
				break;
			}
			case PPPOE_PADT:
			{
				Application::GetInstance().RemoveContact(addr);
				break;
			}
		}
	}
	else if(container.m_pppoeHeader.code == PPPOE_PADO &&
			EtherNetContainer::MacToString((char*)container.m_ethHeader.ether_dhost) == m_mac)
	{
		HostAddress addr((unsigned short)const_cast<PPPoEDContainer&>(container).m_tags[PPPOED_HU].c_str(),
						EtherNetContainer::MacToString((char*)container.m_ethHeader.ether_shost));
		Application::GetInstance().AddContact(addr);
	}
}
