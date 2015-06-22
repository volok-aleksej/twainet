#include <Winsock2.h>
#include "inet_headers.h"

/***********************************************************************************************/
/*                                 EtherNetContainer                                           */
/***********************************************************************************************/
EtherNetContainer::EtherNetContainer()
	: m_child(0){}
EtherNetContainer::EtherNetContainer(const std::string& srcmac, const std::string& dstmac)
	: m_child(0)
{
	for(int i = 0, pos = 0; i < 6; srcmac.size() >= 17 && dstmac.size() >= 17 && i++, pos += 3)
	{
		char mactemp[3] = {0};
		memcpy(mactemp, srcmac.c_str() + pos, 2);
		m_ethHeader.ether_shost[i] = (unsigned char)strtol(mactemp, NULL, 16);
		memcpy(mactemp, dstmac.c_str() + pos, 2);
		m_ethHeader.ether_dhost[i] = (unsigned char)strtol(mactemp, NULL, 16);
	}
}

EtherNetContainer::~EtherNetContainer()
{
	if(m_child)
	{
		delete m_child;
	}
}


bool EtherNetContainer::serialize(char* data, int len)
{
	std::string sdata(data, len);
	if(!SerializeData(sdata))
	{
		return false;
	}

	switch(m_ethHeader.ether_type)
	{
		case ETHERTYPE_PPPOED:
			m_child = new PPPoEDContainer;
			return m_child->serialize(data, len);
		default:
			return false;
	};
}

bool EtherNetContainer::deserialize(char* data, int& len)
{
	std::string sdata;
	if(!DeserializeData(sdata) ||
		len < (int)sdata.size())
	{
		len = sdata.size();
		return false;
	}

	memcpy(data, sdata.c_str(), len);
	return true;
}

bool EtherNetContainer::SerializeData(const std::string& data)
{
	if(data.size() < sizeof(m_ethHeader))
		return false;
	memcpy(&m_ethHeader, data.c_str(), sizeof(m_ethHeader));
	m_ethHeader.ether_type = htons(m_ethHeader.ether_type);
	return true;
}

bool EtherNetContainer::DeserializeData(std::string& data)
{
	data.resize(sizeof(m_ethHeader));
	ether_header ethHeader = m_ethHeader;
	ethHeader.ether_type = htons(ethHeader.ether_type);
	memcpy((void*)data.c_str(), &ethHeader, sizeof(m_ethHeader));
	return true;
}

/***********************************************************************************************/
/*                                    PPPoEContainer                                           */
/***********************************************************************************************/
PPPoEContainer::PPPoEContainer()
{
	m_pppoeHeader.version_type = PPPOE_VERTYPE;
	m_pppoeHeader.sessionId = 0;
	m_pppoeHeader.code = 0;
	m_pppoeHeader.payload = 0;
}
PPPoEContainer::PPPoEContainer(const std::string& srcmac, const std::string& dstmac, unsigned char code)
	: EtherNetContainer(srcmac, dstmac)
{
	m_pppoeHeader.version_type = PPPOE_VERTYPE;
	m_pppoeHeader.sessionId = 0;
	m_pppoeHeader.code = code;
	m_pppoeHeader.payload = 0;
}
PPPoEContainer::~PPPoEContainer(){}

bool PPPoEContainer::SerializeData(const std::string& data)
{
	if(!EtherNetContainer::SerializeData(data) ||
		data.size() < sizeof(m_ethHeader) + sizeof(m_pppoeHeader))
	{
		return false;
	}
	memcpy(&m_pppoeHeader, data.c_str() + sizeof(m_ethHeader), sizeof(m_pppoeHeader));
	m_pppoeHeader .payload = htons(m_pppoeHeader.payload);
	return true;
}

bool PPPoEContainer::DeserializeData(std::string& data)
{
	std::string ethdata;
	if(!EtherNetContainer::DeserializeData(ethdata))
	{
		return false;
	}
	data.resize(ethdata.size() + sizeof(m_pppoeHeader));
	memcpy((void*)data.c_str(), ethdata.c_str(), ethdata.size());
	pppoe_header pppoeheader = m_pppoeHeader;
	pppoeheader .payload = htons(pppoeheader.payload);
	memcpy((void*)(data.c_str() + ethdata.size()), &pppoeheader, sizeof(pppoeheader));
	return true;
}

/***********************************************************************************************/
/*                                   PPPoEDContainer                                           */
/***********************************************************************************************/
PPPoEDContainer::PPPoEDContainer()
{
}

PPPoEDContainer::PPPoEDContainer(const std::string& srcmac, const std::string& dstmac, unsigned char code)
	: PPPoEContainer(srcmac, dstmac, code)
{
	m_ethHeader.ether_type = ETHERTYPE_PPPOED;
}

PPPoEDContainer::~PPPoEDContainer()
{
}

bool PPPoEDContainer::SerializeData(const std::string& data)
{
	return false;
}

bool PPPoEDContainer::DeserializeData(std::string& data)
{
	m_pppoeHeader.payload = 0;
	for(std::map<int, std::string>::iterator it = m_tags.begin();
		it != m_tags.end(); it++)
	{
		m_pppoeHeader.payload += sizeof(pppoe_tag) + it->second.size();
	}

	std::string pppoedata;
	if(!PPPoEContainer::DeserializeData(pppoedata))
	{
		return false;
	}
	data.resize(pppoedata.size() + m_pppoeHeader.payload);
	memcpy((void*)data.c_str(), pppoedata.c_str(), pppoedata.size());
	int pos = pppoedata.size();
	for(std::map<int, std::string>::iterator it = m_tags.begin();
		it != m_tags.end(); it++)
	{
		pppoe_tag tag = {htons(it->first), htons(it->second.size())};
		memcpy((void*)(data.c_str() + pos), &tag, sizeof(tag));
		pos += sizeof(tag);
		memcpy((void*)(data.c_str() + pos), it->second.c_str(), it->second.size());
		pos += it->second.size();
	}
	return true;
}