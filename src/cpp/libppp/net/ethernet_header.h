#ifndef ETHERNET_HEADER_H
#define ETHERNET_HEADER_H

#include "connector_lib\handler\data_message.h"
#include "common\net_structs.h"

class EtherNetContainer : public DataMessage
{
public:
	EtherNetContainer();
	EtherNetContainer(const std::string& srcmac, const std::string& dstmac);
	EtherNetContainer(const EtherNetContainer& container);
	virtual ~EtherNetContainer();

	void operator = (const EtherNetContainer& container);

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

#endif/*ETHERNET_HEADER_H*/