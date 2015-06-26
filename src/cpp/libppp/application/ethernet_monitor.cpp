#include "ethernet_monitor.h"
#include "net\parser_states.h"
#include "common\guid_generator.h"
#include "application\application.h"

#define TIMEOUT_SEND	10
#define MAXIMUM_SEND	5

EthernetMonitor::EthernetMonitor(pcap_t *fp, const std::string& mac)
	: m_fp(fp), m_mac(mac), m_timeoutCount(TIMEOUT_SEND), m_currentPADISend(0)
{
	ManagersContainer::GetInstance().AddManager(static_cast<IManager*>(this));
}

EthernetMonitor::~EthernetMonitor()
{
}

void EthernetMonitor::ManagerStart()
{
	if(!m_fp)
	{
		return;
	}

	bpf_program fcode;
	int res = pcap_compile(m_fp, &fcode, "pppoed or pppoes", 1, 0xffffff);
    res = pcap_setfilter(m_fp, &fcode);
}

void EthernetMonitor::ManagerFunc()
{
	if(!m_fp)
	{
		return;
	}

	//send PADI request with timeout
	if(m_currentPADISend < MAXIMUM_SEND && ++m_timeoutCount >= TIMEOUT_SEND)
	{
		int len = 0;
		unsigned char* data = 0;
		//Send PADI
		PPPoEDContainer padi(m_mac, ETHER_BROADCAST, PPPOE_PADI);
		padi.deserialize(0, len);
		data = new unsigned char[len];
		if(padi.deserialize((char*)data, len))
		{
			pcap_sendpacket(m_fp, data, len);
		}
		delete data;
		data = 0;
		m_timeoutCount = 0;
		m_currentPADISend++;
	}

	//get inet data
	pcap_pkthdr* header;
	const unsigned char* inetdata = 0;
	if (pcap_next_ex(m_fp, &header, &inetdata) < 0)
	{
		Stop();
		return;
	}
	if(!inetdata)
	{
		return;
	}

	BasicState state(this);
	BasicState *nextState = &state;
	while(nextState)
	{
		nextState = nextState->NextState((char*)inetdata, header->len);
	}
}

void EthernetMonitor::ManagerStop()
{
	if(!m_fp)
	{
		return;
	}

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
	m_fp = 0;
}

void EthernetMonitor::OnPacket(const PPPoEDContainer& container)
{
	if(!m_fp || const_cast<PPPoEDContainer&>(container).m_tags[PPPOED_VS] != PPPOED_DEFAULT_VENDOR)
		return;

	if (_stricmp(EtherNetContainer::MacToString((char*)container.m_ethHeader.ether_dhost).c_str(), ETHER_BROADCAST) == 0 &&
		_stricmp(EtherNetContainer::MacToString((char*)container.m_ethHeader.ether_shost).c_str(), m_mac.c_str()) != 0)
	{
		HostAddress addr(*(unsigned short*)const_cast<PPPoEDContainer&>(container).m_tags[PPPOED_HU].c_str(),
						EtherNetContainer::MacToString((char*)container.m_ethHeader.ether_shost));

		switch(container.m_pppoeHeader.code)
		{
			case PPPOE_PADI:
			{
				if(!Application::GetInstance().AddContact(addr))
					break;

				int len = 0;
				PPPoEDContainer pado(m_mac, EtherNetContainer::MacToString((char*)container.m_ethHeader.ether_shost), PPPOE_PADO);
				pado.m_tags[PPPOED_ACC] = CreateGUID();
				pado.deserialize(0, len);
				unsigned char *data = new unsigned char[len];
				if(pado.deserialize((char*)data, len))
				{
					pcap_sendpacket(m_fp, data, len);
				}
				delete data;
				data = 0;

				break;
			}
			case PPPOE_PADT:
			{
				Application::GetInstance().RemoveContact(addr);
				break;
			}
		}
	}
	else if(_stricmp(EtherNetContainer::MacToString((char*)container.m_ethHeader.ether_dhost).c_str(), ETHER_BROADCAST) != 0 &&
			_stricmp(EtherNetContainer::MacToString((char*)container.m_ethHeader.ether_shost).c_str(), m_mac.c_str()) != 0 &&
			container.m_pppoeHeader.code == PPPOE_PADR)
	{
		HostAddress addr(*(unsigned short*)const_cast<PPPoEDContainer&>(container).m_tags[PPPOED_HU].c_str(),
						EtherNetContainer::MacToString((char*)container.m_ethHeader.ether_shost), rand());
		HostAddress orgAddr = Application::GetInstance().GetContact(addr.m_hostId);
		if (orgAddr.m_sessionId != 0 ||
			orgAddr.m_hostCookie != const_cast<PPPoEDContainer&>(container).m_tags[PPPOED_ACC])
			return;

		int len = 0;
		PPPoEDContainer pads(m_mac, EtherNetContainer::MacToString((char*)container.m_ethHeader.ether_shost), PPPOE_PADS);
		pads.m_tags[PPPOED_ACC] = const_cast<PPPoEDContainer&>(container).m_tags[PPPOED_ACC];
		pads.m_pppoeHeader.sessionId = addr.m_sessionId;
		pads.deserialize(0, len);
		unsigned char *data = new unsigned char[len];
		if(pads.deserialize((char*)data, len))
		{
			pcap_sendpacket(m_fp, data, len);
		}
		delete data;
		data = 0;

		addr.m_hostCookie = orgAddr.m_hostCookie;
		Application::GetInstance().UpdateContact(addr);
	}
	else if(_stricmp(EtherNetContainer::MacToString((char*)container.m_ethHeader.ether_dhost).c_str(), m_mac.c_str()) == 0)
	{
		HostAddress addr(*(unsigned short*)const_cast<PPPoEDContainer&>(container).m_tags[PPPOED_HU].c_str(),
						EtherNetContainer::MacToString((char*)container.m_ethHeader.ether_shost),
						container.m_pppoeHeader.sessionId);

		switch(container.m_pppoeHeader.code)
		{
			case PPPOE_PADO:
			{
				if(!Application::GetInstance().AddContact(addr) &&
					( Application::GetInstance().GetContact(addr.m_hostId).m_sessionId != 0||
					addr.m_hostId < getCpuHash()))
					break;

				int len = 0;
				PPPoEDContainer padr(m_mac, EtherNetContainer::MacToString((char*)container.m_ethHeader.ether_shost), PPPOE_PADR);
				padr.m_tags[PPPOED_ACC] = const_cast<PPPoEDContainer&>(container).m_tags[PPPOED_ACC];
				padr.deserialize(0, len);
				unsigned char *data = new unsigned char[len];
				if(padr.deserialize((char*)data, len))
				{
					pcap_sendpacket(m_fp, data, len);
				}
				delete data;
				data = 0;

				addr.m_hostCookie = padr.m_tags[PPPOED_ACC];
				addr.m_hostName = const_cast<PPPoEDContainer&>(container).m_tags[PPPOED_AN];
				Application::GetInstance().UpdateContact(addr);

				break;
			}
			case PPPOE_PADS:
			{
				Application::GetInstance().UpdateContact(addr);
				break;
			}
		}
	}
}
