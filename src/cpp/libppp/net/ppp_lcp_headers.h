#ifndef PPP_LCP_HEADERS_H
#define PPP_LCP_HEADERS_H

#include "pppoe_headers.h"

class PPPLCPContainer : public PPPoESContainer
{
public:
	PPPLCPContainer();
	PPPLCPContainer(const std::string& srcmac, const std::string& dstmac, unsigned short sessionId, unsigned char code);
	virtual ~PPPLCPContainer();
protected:
	virtual int SerializeData(const std::string& data);
	virtual int DeserializeData(std::string& data);
public:
	ppp_lcp_header m_lcpHeader;
};

#endif/*PPP_LCP_HEADERS_H*/