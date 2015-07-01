#ifndef PPP_CONNECTION_H
#define PPP_CONNECTION_H

#include "thread_lib\common\managers_container.h"

class PPPConnection : public DynamicManager
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
	PPPConnection(const std::string& hostId, bool isServer);
	~PPPConnection();

protected:
	void ManagerFunc();
	void ManagerStart();
	void ManagerStop();
private:
	std::string m_hostId;
	ConnectionState m_state;
	LCPState m_lcpState;
	bool m_isServer;
};

#endif/*PPP_CONNECTION_H*/