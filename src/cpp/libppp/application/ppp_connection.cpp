#include "ppp_connection.h"

PPPConnection::PPPConnection(const std::string& hostId, bool isServer)
	: m_hostId(hostId), m_state(OFF)
	, m_lcpState(LCP_BEGIN), m_isServer(isServer)
{
	ManagersContainer::GetInstance().AddManager(static_cast<IManager*>(this));
}

PPPConnection::~PPPConnection()
{
}

void PPPConnection::ManagerFunc()
{
}

void PPPConnection::ManagerStart()
{
}

void PPPConnection::ManagerStop()
{
}
