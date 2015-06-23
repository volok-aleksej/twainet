#include "ethernet_monitor.h"
#include "net\inet_headers.h"

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
	padi.m_tags.insert(std::make_pair(PPPOED_VS, PPPOED_VENDOR));
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

		//TODO(): parser
	}
}

void EthernetMonitor::Stop()
{
	int len = 0;
	PPPoEDContainer padt(m_mac, "ff:ff:ff:ff:ff:ff", PPPOE_PADT);
	padt.m_tags.insert(std::make_pair(PPPOED_VS, PPPOED_VENDOR));
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
