#include <Winsock2.h>
#include "ppp_lcp_headers.h"

/***********************************************************************************************/
/*                                   PPPLCPContainer                                           */
/***********************************************************************************************/
PPPLCPContainer::PPPLCPContainer()
{
	m_protocol = PPPOES_LCP;
}

PPPLCPContainer::PPPLCPContainer(const std::string& srcmac, const std::string& dstmac, unsigned short sessionId, unsigned char code)
	: PPPoESContainer(srcmac, dstmac, sessionId, PPPOES_LCP)
{
}

PPPLCPContainer::PPPLCPContainer(const PPPLCPContainer& container)
	: PPPoESContainer(container)
{
	operator = (container);
}

PPPLCPContainer::~PPPLCPContainer()
{
}

EtherNetContainer* PPPLCPContainer::Clone()
{
	return new PPPLCPContainer(*this);
}

void PPPLCPContainer::operator = (const PPPLCPContainer& container)
{
	m_lcpHeader = container.m_lcpHeader;
}

int PPPLCPContainer::SerializeData(const std::string& data)
{
	int serialLen = PPPoESContainer::SerializeData(data);
	if(serialLen == 0||
		data.size() < serialLen + sizeof(ppp_lcp_header))
	{
		return 0;
	}

	memcpy(&m_lcpHeader, data.c_str() + serialLen, sizeof(ppp_lcp_header));
	m_lcpHeader.len = htons(m_lcpHeader.len);
	return serialLen + sizeof(ppp_lcp_header);
}

int PPPLCPContainer::DeserializeData(std::string& data)
{
	std::string pppoesdata;
	if(!PPPoESContainer::DeserializeData(pppoesdata))
	{
		return 0;
	}
	
	data.resize(pppoesdata.size() + sizeof(ppp_lcp_header));
	memcpy((void*)data.c_str(), pppoesdata.c_str(), pppoesdata.size());
	ppp_lcp_header header = m_lcpHeader;
	header.len = htons(m_lcpHeader.len);
	memcpy((char*)data.c_str() + pppoesdata.size(), &header, sizeof(ppp_lcp_header));
	return pppoesdata.size() + sizeof(ppp_lcp_header);
}

/***********************************************************************************************/
/*                                  PPPLCPConfContainer                                        */
/***********************************************************************************************/
PPPLCPConfContainer::PPPLCPConfContainer()
{
}

PPPLCPConfContainer::PPPLCPConfContainer(const std::string& srcmac, const std::string& dstmac, unsigned short sessionId, ConfType type)
	: PPPLCPContainer(srcmac, dstmac, sessionId, type)
{
}

PPPLCPConfContainer::PPPLCPConfContainer(const PPPLCPConfContainer& container)
	: PPPLCPContainer(container)
{
	operator = (container);
}

PPPLCPConfContainer::~PPPLCPConfContainer()
{
}

EtherNetContainer* PPPLCPConfContainer::Clone()
{
	return new PPPLCPConfContainer(*this);
}

void PPPLCPConfContainer::operator = (const PPPLCPConfContainer& container)
{
	m_options = container.m_options;
}

int PPPLCPConfContainer::SerializeData(const std::string& data)
{
	return 0;
}

int PPPLCPConfContainer::DeserializeData(std::string& data)
{
	return 0;
}
