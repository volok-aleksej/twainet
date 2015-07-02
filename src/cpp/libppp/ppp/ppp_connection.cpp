#include "ppp_connection.h"

PPPConnection::PPPConnection(EthernetMonitor* monitor, const std::string& hostId)
	: PPPoEConnection(monitor, hostId), m_statePPP(OFF), m_stateLCP(LCP_BEGIN), m_isServer(false)
{
	ManagersContainer::GetInstance().AddManager(static_cast<IManager*>(this));
}

PPPConnection::~PPPConnection()
{
}

void PPPConnection::ManagerFunc()
{
	PPPoEConnection::ManagerFunc();
}