#include <Winsock2.h>
#include "pppoe_headers.h"
#include "application\application.h"

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

int PPPoEContainer::SerializeData(const std::string& data)
{
	int serialLen = EtherNetContainer::SerializeData(data);
	if(serialLen == 0 ||
		data.size() < serialLen + sizeof(m_pppoeHeader))
	{
		return 0;
	}
	memcpy(&m_pppoeHeader, data.c_str() + serialLen, sizeof(m_pppoeHeader));
	m_pppoeHeader.payload = htons(m_pppoeHeader.payload);
	return serialLen + sizeof(m_pppoeHeader);
}

int PPPoEContainer::DeserializeData(std::string& data)
{
	std::string ethdata;
	if(!EtherNetContainer::DeserializeData(ethdata))
	{
		return 0;
	}
	data.resize(ethdata.size() + sizeof(m_pppoeHeader));
	memcpy((void*)data.c_str(), ethdata.c_str(), ethdata.size());
	pppoe_header pppoeheader = m_pppoeHeader;
	pppoeheader.payload = htons(pppoeheader.payload);
	memcpy((void*)(data.c_str() + ethdata.size()), &pppoeheader, sizeof(pppoeheader));
	return data.size();
}

/***********************************************************************************************/
/*                                   PPPoEDContainer                                           */
/***********************************************************************************************/
PPPoEDContainer::PPPoEDContainer()
{
	m_ethHeader.ether_type = ETHERTYPE_PPPOED;
	std::string cpuhash = Application::GetInstance().GetOwnId();
	char compName[200] = {0};
	DWORD sizeName = sizeof(compName);
	GetComputerNameA(compName, &sizeName);

	m_tags.insert(std::make_pair(PPPOED_HU, cpuhash));
	m_tags.insert(std::make_pair(PPPOED_VS, PPPOED_DEFAULT_VENDOR));
	m_tags.insert(std::make_pair(PPPOED_AN, compName));
}

PPPoEDContainer::PPPoEDContainer(const std::string& srcmac, const std::string& dstmac, unsigned char code)
	: PPPoEContainer(srcmac, dstmac, code)
{
	m_ethHeader.ether_type = ETHERTYPE_PPPOED;
	std::string cpuhash = Application::GetInstance().GetOwnId();
	char compName[200] = {0};
	DWORD sizeName = sizeof(compName);
	GetComputerNameA(compName, &sizeName);

	m_tags.insert(std::make_pair(PPPOED_HU, cpuhash));
	m_tags.insert(std::make_pair(PPPOED_VS, PPPOED_DEFAULT_VENDOR));
	m_tags.insert(std::make_pair(PPPOED_AN, compName));
}

PPPoEDContainer::~PPPoEDContainer()
{
}

int PPPoEDContainer::SerializeData(const std::string& data)
{
	int serialLen = PPPoEContainer::SerializeData(data);
	if(serialLen == 0||
		(int)data.size() < serialLen + m_pppoeHeader.payload)
	{
		return 0;
	}
	
	m_tags.clear();

	char* sdata = (char*)data.c_str() + serialLen;
	int pos = 0;
	while(pos < m_pppoeHeader.payload)
	{
		unsigned short type;
		unsigned short len;

		if(pos + 2 > m_pppoeHeader.payload)
			break;
		memcpy(&type, sdata + pos, 2);
		pos += 2;

		if(pos + 2 > m_pppoeHeader.payload)
			break;
		memcpy(&len, sdata + pos, 2);
		pos += 2;
		
		type = htons(type);
		len = htons(len);

		if(pos + len > m_pppoeHeader.payload)
			break;
		std::string value(len, 0);
		memcpy((char*)value.c_str(), sdata + pos, len);
		switch(type)
		{
			case PPPOED_VS:
			{
				pppoed_tag_vendor vendor;
				memcpy(&vendor, value.c_str(), sizeof(vendor));
				vendor.id = htonl(vendor.id);
				memcpy((char*)value.c_str(), &vendor, sizeof(vendor));
				break;
			}
		}
		
		m_tags.insert(std::make_pair(type, value));
		pos += len;
	}

	return serialLen + m_pppoeHeader.payload;
}

int PPPoEDContainer::DeserializeData(std::string& data)
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
		return 0;
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
		std::string value(it->second.size(), 0);
		switch(it->first)
		{
			case PPPOED_VS:
			{
				pppoed_tag_vendor vendor;
				memcpy(&vendor, it->second.c_str(), it->second.size());
				vendor.id = htonl(vendor.id);
				memcpy((char*)value.c_str(), &vendor, value.size());
				break;
			}
			default:
			{
				memcpy((char*)value.c_str(), it->second.c_str(), it->second.size());
				break;
			}
		}
		memcpy((void*)(data.c_str() + pos), value.c_str(), value.size());
		pos += it->second.size();
	}
	return pppoedata.size() + m_pppoeHeader.payload;
}

/***********************************************************************************************/
/*                                   PPPoESContainer                                           */
/***********************************************************************************************/
PPPoESContainer::PPPoESContainer()
{
	m_ethHeader.ether_type = ETHERTYPE_PPPOES;
	m_pppoeHeader.code = 0;
}

PPPoESContainer::PPPoESContainer(const std::string& srcmac, const std::string& dstmac, unsigned short sessionId, unsigned short protocol)
	: PPPoEContainer(srcmac, dstmac, 0)
{
	m_ethHeader.ether_type = ETHERTYPE_PPPOES;
	m_pppoeHeader.sessionId = sessionId;
	m_protocol = protocol;
}

PPPoESContainer::~PPPoESContainer()
{
}

int PPPoESContainer::SerializeData(const std::string& data)
{
	int serialLen = PPPoEContainer::SerializeData(data);
	if(serialLen == 0||
		data.size() < serialLen + sizeof(unsigned short))
	{
		return 0;
	}

	unsigned short protocol;
	memcpy(&protocol, data.c_str() + serialLen, sizeof(unsigned short));
	m_protocol = htons(protocol);
	return serialLen + sizeof(unsigned short);
}

int PPPoESContainer::DeserializeData(std::string& data)
{
	std::string pppoedata;
	if(!PPPoEContainer::DeserializeData(pppoedata))
	{
		return 0;
	}
	
	data.resize(pppoedata.size() + sizeof(unsigned short));
	memcpy((void*)data.c_str(), pppoedata.c_str(), pppoedata.size());
	unsigned short protocol = htons(m_protocol);
	memcpy((char*)data.c_str() + pppoedata.size(), &m_protocol, sizeof(unsigned short));
	return pppoedata.size() + sizeof(unsigned short);
}