#ifndef PPP_CONNECTION_H
#define PPP_CONNECTION_H

#include "pppoe_connection.h"

class PPPConnection;

typedef ConnectionPacket<PPPoESContainer, PPPConnection> PPPoESPacket;

class PPPConnection : public PPPoEConnection
{
public:
	enum ConnectionState
	{
		OFF = 0,
		CONNECTION,
		AUTH,
		NETWORK,
		CLOSING
	};

	enum LCPState
	{
		LCP_BEGIN = 0,
		LCP_START,
		LCP_CLOSED,
		LCP_STOPED,
		LCP_CLOSING,
		LCP_STOPING,
		LCP_SENDR,
		LCP_RECVC,
		LCP_SENDC
	};
public:
	PPPConnection(EthernetMonitor* monitor, const std::string& hostId);
	~PPPConnection();

protected:
	void ManagerFunc();
	void ManagerStop();
	bool IsConnectionPacket(IConnectionPacket* packet);
protected:
	ConnectionState m_statePPP;
	LCPState m_stateLCP;
	bool m_isServer;

private:
	template<typename Packet, typename Connection> friend class ConnectionPacket;
	void OnPacket(PPPoESContainer* container);
};

#endif/*PPP_CONNECTION_H*/