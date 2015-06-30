#include "ppp_lcp_headers.h"

PPPLCPContainer::PPPLCPContainer()
{
	m_protocol = PPPOES_LCP;
}

PPPLCPContainer::PPPLCPContainer(const std::string& srcmac, const std::string& dstmac, unsigned short sessionId, unsigned char code)
	: PPPoESContainer(srcmac, dstmac, sessionId, PPPOES_LCP)
{
}

PPPLCPContainer::~PPPLCPContainer()
{
}

int PPPLCPContainer::SerializeData(const std::string& data)
{
	return 0;
}

int PPPLCPContainer::DeserializeData(std::string& data)
{
	return 0;
}