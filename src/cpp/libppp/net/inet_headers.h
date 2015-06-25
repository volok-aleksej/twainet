#ifndef INET_HEADERS_H
#define INET_HEADERS_H

#include "connector_lib\handler\data_message.h"
#include "common\net_structs.h"
#include <map>

class EtherNetContainer : public DataMessage
{
public:
	EtherNetContainer();
	EtherNetContainer(const std::string& srcmac, const std::string& dstmac);
	virtual ~EtherNetContainer();

	static std::string StringToMac(const std::string& str);
	static std::string MacToString(const char mac[6]);
public:
	virtual bool serialize(char* data, int len);
	virtual bool deserialize(char* data, int& len);
protected:
	virtual int SerializeData(const std::string& data);
	virtual int DeserializeData(std::string& data);
public:
	ether_header m_ethHeader;
};

class PPPoEContainer : public EtherNetContainer
{
public:
	PPPoEContainer();
	PPPoEContainer(const std::string& srcmac, const std::string& dstmac, unsigned char code);
	virtual ~PPPoEContainer();
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
	virtual ~PPPoEDContainer();
protected:
	virtual int SerializeData(const std::string& data);
	virtual int DeserializeData(std::string& data);
public:
	std::map<int, std::string> m_tags;
};

#endif/*INET_HEADERS_H*/