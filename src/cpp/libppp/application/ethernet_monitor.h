#ifndef ETHERNET_MONITOR_H
#define ETHERNET_MONITOR_H

#include "net\ppp_lcp_headers.h"
#include "parser\parser_states.h"
#include "parser\net_packet.h"
#include "pcap.h"
#include "thread_lib\common\object_manager.h"
#include <string>
#include <vector>

class PPPoEConnection;

class EthernetMonitor : public IConnection
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

		PPPoEConnection* m_connection;
		std::string m_hostId;
	};

	template<typename TClass, typename TFunc, typename TObject> friend class ReferenceObject;
	template<typename TClass, typename TFunc> friend class Reference;
	void ListConnection(const std::vector<std::string>& list, const PPPoEConnection* connection);
	bool CheckConnection(const Contact& contact, const PPPoEConnection* connection);
	bool RemoveConnection(const PPPoEConnection* connection);
	void ConnectorPacket(const IConnectionPacket& packet, const PPPoEConnection* connection);
protected:
	friend class BasicState;
	void OnPacket(const IConnectionPacket& packet);
protected:
	void OnPacket(PPPoEDContainer* packet);
	bool IsConnectionPacket(PPPoEDContainer* packet);
public:
	template<typename Packet, typename Connection, typename void (Connection::*ConnectionFunc)(Packet*)> friend class ConnectionPacket;
	typedef ConnectionPacket<PPPoEDContainer, EthernetMonitor, &EthernetMonitor::OnPacket> PPPoEDMonitorPacket;
protected:
	friend class Application;
	void MonitorStart();
	bool MonitorFunc();
	void MonitorStop();
private:
	pcap_t *m_fp;
	std::string m_mac;
	ObjectManager<PPPoEConnection*> m_contacts;

	//for PADI request time in second
	int m_timeoutCount;
	unsigned int m_currentClock;
};


#endif/*ETHERNET_MONITOR_H*/