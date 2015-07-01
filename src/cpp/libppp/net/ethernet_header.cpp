#include <Winsock2.h>
#include "ethernet_header.h"

EtherNetContainer::EtherNetContainer(){}
EtherNetContainer::EtherNetContainer(const std::string& srcmac, const std::string& dstmac)
{
	memcpy(m_ethHeader.ether_shost, StringToMac(srcmac).c_str(), sizeof(m_ethHeader.ether_shost));
	memcpy(m_ethHeader.ether_dhost, StringToMac(dstmac).c_str(), sizeof(m_ethHeader.ether_dhost));
}

EtherNetContainer::EtherNetContainer(const EtherNetContainer& container)
{
	operator = (container);
}

EtherNetContainer::~EtherNetContainer()
{
}

void EtherNetContainer::operator = (const EtherNetContainer& container)
{
	m_ethHeader = container.m_ethHeader;
}

std::string EtherNetContainer::StringToMac(const std::string& str)
{
	char mac[6] = {0};
	for(int i = 0, pos = 0; i < 6; str.size() >= 17 && i++, pos += 3)
	{
		char mactemp[3] = {0};
		memcpy(mactemp, str.c_str() + pos, 2);
		mac[i] = (unsigned char)strtol(mactemp, NULL, 16);
	}

	return std::string(mac, 6);
}

std::string EtherNetContainer::MacToString(const char mac[6])
{
	std::string str(17, 0);
	for(int i = 0, pos = 0; i < 6; str.size() >= 17 && i++, pos += 3)
	{
		if(i != 5)
			sprintf((char*)(str.c_str() + pos), "%02X:", (unsigned char)mac[i]);
		else
			sprintf((char*)(str.c_str() + pos), "%02X", (unsigned char)mac[i]);
	}
	return str;
}

bool EtherNetContainer::serialize(char* data, int len)
{
	std::string sdata(data, len);
	return SerializeData(sdata) != 0;
}

bool EtherNetContainer::deserialize(char* data, int& len)
{
	std::string sdata;
	int deserialLen = DeserializeData(sdata);
	if(len < deserialLen)
	{
		len = deserialLen;
		return false;
	}

	memcpy(data, sdata.c_str(), len);
	return true;
}

int EtherNetContainer::SerializeData(const std::string& data)
{
	if(data.size() < sizeof(m_ethHeader))
		return 0;
	memcpy(&m_ethHeader, data.c_str(), sizeof(m_ethHeader));
	m_ethHeader.ether_type = htons(m_ethHeader.ether_type);
	return sizeof(m_ethHeader);
}

int EtherNetContainer::DeserializeData(std::string& data)
{
	data.resize(sizeof(m_ethHeader));
	ether_header ethHeader = m_ethHeader;
	ethHeader.ether_type = htons(ethHeader.ether_type);
	memcpy((void*)data.c_str(), &ethHeader, sizeof(m_ethHeader));
	return sizeof(m_ethHeader);
}
