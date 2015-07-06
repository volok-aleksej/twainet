#ifndef PPP_LCP_HEADERS_H
#define PPP_LCP_HEADERS_H

#include "pppoe_headers.h"

class PPPLCPContainer : public PPPoESContainer
{
public:
	PPPLCPContainer();
	PPPLCPContainer(const std::string& srcmac, const std::string& dstmac, unsigned short sessionId, unsigned char code);
	PPPLCPContainer(const PPPLCPContainer& container);
	virtual ~PPPLCPContainer();

	virtual EtherNetContainer* Clone();
	void operator = (const PPPLCPContainer& container);
protected:
	virtual int SerializeData(const std::string& data);
	virtual int DeserializeData(std::string& data);
public:
	ppp_lcp_header m_lcpHeader;
};

class PPPLCPConfContainer : public PPPLCPContainer
{
	enum ConfType
	{
		CONF_REQUEST = PPPLCP_CR,
		CONF_ASK = PPPLCP_CA,
		CONF_NO_CONFIRM = PPPLCP_CNC,
		CONF_RESET = PPPLCP_CNR
	};
public:
	PPPLCPConfContainer();
	PPPLCPConfContainer(const std::string& srcmac, const std::string& dstmac, unsigned short sessionId, ConfType type);
	PPPLCPConfContainer(const PPPLCPConfContainer& container);
	virtual ~PPPLCPConfContainer();

	virtual EtherNetContainer* Clone();
	void operator = (const PPPLCPConfContainer& container);
protected:
	virtual int SerializeData(const std::string& data);
	virtual int DeserializeData(std::string& data);
public:
	ConfType m_type;
	std::map<unsigned char, std::string> m_options;
};

#endif/*PPP_LCP_HEADERS_H*/