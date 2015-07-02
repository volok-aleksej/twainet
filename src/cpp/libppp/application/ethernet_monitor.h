#ifndef ETHERNET_MONITOR_H
#define ETHERNET_MONITOR_H

#include "net\pppoe_headers.h"
#include "net\parser_states.h"
#include "pcap.h"
#include "thread_lib\common\object_manager.h"
#include <string>
#include <vector>

class PPPoEDConnection;

class EthernetMonitor
{
public:
	EthernetMonitor(pcap_t *fp, const std::string& mac);
	virtual ~EthernetMonitor();
	
	bool AddContact(const std::string& hostId);
	bool RemoveContact(const std::string& hostId);

	std::string GetMac();
	void SendPacket(unsigned char* data, int len);
protected:
	class Contact
	{
	public:
		Contact(const std::string& hostId)
			: m_connection(0), m_hostId(hostId){}
		~Contact(){}

		PPPoEDConnection* m_connection;
		std::string m_hostId;
	};

	template<typename TClass, typename TFunc, typename TObject> friend class ReferenceObject;
	template<typename TClass, typename TFunc> friend class Reference;
	void ListConnection(const std::vector<std::string>& list, const PPPoEDConnection* connection);
	bool CheckConnection(const Contact& contact, const PPPoEDConnection* connection);
	bool RemoveConnection(const PPPoEDConnection* connection);
	void ConnectorPacket(const PPPoEDContainer& container, const PPPoEDConnection* connection);
protected:
	friend class BasicState;
	void OnPacket(const PPPoEDContainer& container);

protected:
	friend class Application;
	void MonitorStart();
	bool MonitorFunc();
	void MonitorStop();
private:
	pcap_t *m_fp;
	std::string m_mac;
	ObjectManager<PPPoEDConnection*> m_contacts;

	//for PADI request time in seconds
	int m_timeoutCount;
	unsigned int m_currentClock;
	
};

#endif/*ETHERNET_MONITOR_H*/