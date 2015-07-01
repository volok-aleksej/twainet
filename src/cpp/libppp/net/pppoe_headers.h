#ifndef PPPOE_HEADERS_H
#define PPPOE_HEADERS_H

#include"ethernet_header.h"
#include <map>

class PPPoEContainer : public EtherNetContainer
{
public:
	PPPoEContainer();
	PPPoEContainer(const std::string& srcmac, const std::string& dstmac, unsigned char code);
	PPPoEContainer(const PPPoEContainer& container);
	virtual ~PPPoEContainer();

	void operator = (const PPPoEContainer& container);
protected:
	virtual int SerializeData(const std::string& data);
	virtual int DeserializeData(std::string& data);
public:
	pppoe_header m_pppoeHeader;
};

class PPPoEDContainer : public PPPoEContainer
{
public:
	PPPoEDContainer();
	PPPoEDContainer(const std::string& srcmac, const std::string& dstmac, unsigned char code);
	PPPoEDContainer(const PPPoEDContainer& container);
	virtual ~PPPoEDContainer();

	void operator = (const PPPoEDContainer& container);
protected:
	virtual int SerializeData(const std::string& data);
	virtual int DeserializeData(std::string& data);
public:
	std::map<int, std::string> m_tags;
};

class PPPoESContainer : public PPPoEContainer
{
public:
	PPPoESContainer();
	PPPoESContainer(const std::string& srcmac, const std::string& dstmac, unsigned short sessionId, unsigned short protocol);
	PPPoESContainer(const PPPoESContainer& container);
	virtual ~PPPoESContainer();

	void operator = (const PPPoESContainer& container);
protected:
	virtual int SerializeData(const std::string& data);
	virtual int DeserializeData(std::string& data);
public:
	unsigned short m_protocol;
};

#endif/*PPPOE_HEADERS_H*/