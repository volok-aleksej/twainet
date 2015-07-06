#include "ppp_connection.h"

PPPConnection::PPPConnection(EthernetMonitor* monitor, const std::string& hostId)
	: PPPoEConnection(monitor, hostId), m_statePPP(OFF), m_stateLCP(LCP_BEGIN), m_isServer(false)
{
	ManagersContainer::GetInstance().AddManager(static_cast<IManager*>(this));
}

PPPConnection::~PPPConnection()
{
}

bool PPPConnection::IsConnectionPacket(IConnectionPacket* packet)
{
	if(!IConnection::IsConnectionPacket(packet))
		return false;
	
	PPPoESPacket* pppoes = dynamic_cast<PPPoESPacket*>(packet);
	if(!pppoes)
		return PPPoEConnection::IsConnectionPacket(packet);

	PPPoESContainer* container = pppoes->GetPacket();
	if(!container)
		return false;
		
	return GetSessionId() == container->m_pppoeHeader.sessionId;
}

void PPPConnection::OnPacket(PPPoESContainer* container)
{
	PPPoESSelfPacket* packet = new PPPoESSelfPacket(container);
	m_containers.AddObject(packet);
}

void PPPConnection::OnContainer(PPPoESContainer* container)
{
}
