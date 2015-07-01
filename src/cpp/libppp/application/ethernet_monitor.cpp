#include "application.h"
#include "ppp_connection.h"
#include "ethernet_monitor.h"
#include "net\parser_states.h"
#include "common\ref.h"
#include "common\guid_generator.h"
#include "ppp\pppoed_connection.h"

#define TIMEOUT_SEND	10
#define MAXIMUM_SEND	5

EthernetMonitor::EthernetMonitor(pcap_t *fp, const std::string& mac)
	: m_fp(fp), m_mac(mac), m_timeoutCount(TIMEOUT_SEND)
	, m_currentPADISend(0), m_currentClock((GetTickCount()/1000))
{
}

EthernetMonitor::~EthernetMonitor()
{
}

std::string EthernetMonitor::GetMac()
{
	return m_mac;
}

void EthernetMonitor::SendPacket(unsigned char* data, int len)
{
	if(!m_fp)
	{
		return;
	}

	pcap_sendpacket(m_fp, data, len);
}

void EthernetMonitor::MonitorStart()
{
	if(!m_fp)
	{
		return;
	}

	bpf_program fcode;
	int res = pcap_compile(m_fp, &fcode, "pppoed or pppoes", 1, 0xffffff);
    res = pcap_setfilter(m_fp, &fcode);
}

bool EthernetMonitor::MonitorFunc()
{
	if(!m_fp)
	{
		return false;
	}

	//send PADI request with timeout
	if(m_currentPADISend < MAXIMUM_SEND && m_timeoutCount >= TIMEOUT_SEND)
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
		m_currentPADISend++;
		m_timeoutCount = 0;
	}
	else if(m_currentClock != (GetTickCount()/1000))
	{
		m_currentClock  = (GetTickCount()/1000);
		m_timeoutCount++;
	}

	//get inet data
	pcap_pkthdr* header;
	const unsigned char* inetdata = 0;
	if (pcap_next_ex(m_fp, &header, &inetdata) < 0)
	{
		return false;
	}
	if(!inetdata)
	{
		return true;
	}

	BasicState state(this);
	BasicState *nextState = &state;
	while(nextState)
	{
		nextState = nextState->NextState((char*)inetdata, header->len);
	}

	return true;
}

void EthernetMonitor::MonitorStop()
{
	if(!m_fp)
	{
		return;
	}
	
	m_contacts.CheckObjects(Ref(this, &EthernetMonitor::RemoveConnection));

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

bool EthernetMonitor::RemoveConnection(const PPPoEDConnection* connection)
{
	const_cast<PPPoEDConnection*>(connection)->Stop();
	return true;
}

bool EthernetMonitor::CheckConnection(const EthernetMonitor::Contact& contact, const PPPoEDConnection* connection)
{
	if(contact.m_hostId == connection->GetHostId())
	{
		const_cast<EthernetMonitor::Contact&>(contact).m_connection = const_cast<PPPoEDConnection*>(connection);
		return true;
	}

	return false;
}

void EthernetMonitor::ListConnection(const std::vector<std::string>& list, const PPPoEDConnection* connection)
{
	const_cast<std::vector<std::string>&>(list).push_back(connection->GetHostId());
}

void EthernetMonitor::ConnectorPacket(const PPPoEDContainer& container, const PPPoEDConnection* connection)
{
	std::string hostId = const_cast<PPPoEDContainer&>(container).m_tags[PPPOED_HU];
	if(connection->GetHostId() == hostId)
	{
		const_cast<PPPoEDConnection*>(connection)->OnPacket(container);
	}
}

bool EthernetMonitor::AddContact(const std::string& hostId)
{
	Contact contact(hostId);
	m_contacts.ProcessingObjects(Ref(this, &EthernetMonitor::CheckConnection, contact));
	if(!contact.m_connection)
	{
		return m_contacts.AddObject(new PPPoEDConnection(this, hostId));
	}

	return false;	
}

bool EthernetMonitor::RemoveContact(const std::string& hostId)
{
	Contact contact(hostId);
	m_contacts.CheckObjects(Ref(this, &EthernetMonitor::CheckConnection, contact));
	if(contact.m_connection)
	{
		contact.m_connection->Stop();
	}

	return true;
}

void EthernetMonitor::OnPacket(const PPPoEDContainer& container)
{
	if(!m_fp || const_cast<PPPoEDContainer&>(container).m_tags[PPPOED_VS] != PPPOED_DEFAULT_VENDOR)
		return;
	
	std::string hostId = const_cast<PPPoEDContainer&>(container).m_tags[PPPOED_HU];
	AddContact(hostId);
	m_contacts.ProcessingObjects(Ref(this, &EthernetMonitor::ConnectorPacket, container));
}
