#include "pppoe_connection.h"
#include "common\ref.h"
#include "application\ethernet_monitor.h"
#include "application\application.h"

std::string RandString(int size);

PPPoEConnection::PPPoEConnection(EthernetMonitor* monitor, const std::string& hostId)
	: m_hostId(hostId), m_sessionId(0), m_statePPPoS(PADI), m_monitor(monitor){}

PPPoEConnection::~PPPoEConnection()
{
}

std::string PPPoEConnection::GetHostId() const
{
	return m_hostId;
}

bool PPPoEConnection::operator == (const PPPoEConnection& addr) const
{
	bool retSession = false;
	if(m_sessionId && m_hostId.empty())
		return m_sessionId == addr.m_sessionId;
	else if(m_sessionId)
		return m_sessionId == addr.m_sessionId && m_hostId == addr.m_hostId;
	else if(m_hostId.empty())
		return false;
	else
		return addr.m_hostId == m_hostId;
}

bool PPPoEConnection::DeleteContainer(const PPPoEContainer* pppoed)
{
	delete pppoed;
	return true;
}

bool PPPoEConnection::GetContainer(PPPoEContainer** const container, const PPPoEContainer* pppoe)
{
	if(*container)
		return false;
	
	*const_cast<PPPoEContainer**>(container) = const_cast<PPPoEContainer*>(pppoe);
	return true;
}

void PPPoEConnection::OnPacket(const PPPoEDContainer& pppoed)
{
	m_containers.AddObject(new PPPoEDContainer(pppoed));
}

void PPPoEConnection::OnPacket(const PPPoESContainer& pppoes)
{
	m_containers.AddObject(new PPPoESContainer(pppoes));
}

void PPPoEConnection::ManagerFunc()
{
	PPPoEContainer* pppoe = 0;
	m_containers.CheckObjects(Ref(this, &PPPoEConnection::GetContainer, &pppoe));
	if(!pppoe)
	{
		return;
	}

	OnContainer(dynamic_cast<PPPoEDContainer*>(pppoe));
	delete pppoe;
}

void PPPoEConnection::OnContainer(PPPoEDContainer* container)
{
	if(!container)
	{
		return;
	}

	m_mac = EtherNetContainer::MacToString((const char*)container->m_ethHeader.ether_shost);
	m_hostName = container->m_tags[PPPOED_AN];

	//get PADI request
	if(container->m_pppoeHeader.code == PPPOE_PADI && m_statePPPoS == PADI)
	{
		PPPoEDContainer pado(m_monitor->GetMac(), m_mac, PPPOE_PADO);
		pado.m_tags[PPPOED_ACC] = m_hostCookie = RandString(16);
		SendPPPoED(pado);
		m_statePPPoS = PADO;
	}

	//get PADO request
	if(container->m_pppoeHeader.code == PPPOE_PADO)
	{
		if (m_statePPPoS == PADI || 
			m_statePPPoS == PADO && container->m_tags[PPPOED_HU] < Application::GetInstance().GetOwnId())
		{
			PPPoEDContainer padr(m_monitor->GetMac(), m_mac, PPPOE_PADR);
			padr.m_tags[PPPOED_ACC] = m_hostCookie = container->m_tags[PPPOED_ACC];
			SendPPPoED(padr);
			m_statePPPoS = PADR;
		}
	}

	//get PADR request
	if(container->m_pppoeHeader.code == PPPOE_PADR)
	{
		switch(m_statePPPoS)
		{
			case PADO:
			{
				if(container->m_tags[PPPOED_ACC] != m_hostCookie)
				{
					m_monitor->RemoveContact(m_hostId);
				}

				PPPoEDContainer pads(m_monitor->GetMac(), m_mac, PPPOE_PADS);
				pads.m_tags[PPPOED_ACC] = m_hostCookie;
				pads.m_pppoeHeader.sessionId = m_sessionId = (unsigned short)GetTickCount();
				SendPPPoED(pads);
				m_statePPPoS = PADS;
				break;
			}
			case PADI:
			{
				m_monitor->RemoveContact(m_hostId);
				break;
			}
		}
	}
	
	//get PADS request
	if(container->m_pppoeHeader.code == PPPOE_PADS)
	{
		switch(m_statePPPoS)
		{
			case PADR:
			{
				if(container->m_tags[PPPOED_ACC] != m_hostCookie)
				{
					m_monitor->RemoveContact(m_hostId);
				}

				m_sessionId = container->m_pppoeHeader.sessionId;
				m_statePPPoS = PADS;
				
				//TODO(): initiate lcp connection
				
				break;
			}
			case PADI:
			{
				m_monitor->RemoveContact(m_hostId);
				break;
			}
		}
	}
}

void PPPoEConnection::ManagerStart()
{
}

void PPPoEConnection::ManagerStop()
{
	m_containers.CheckObjects(Ref(this, &PPPoEConnection::DeleteContainer));
}

void PPPoEConnection::SendPPPoED(PPPoEDContainer container)
{
	int len = 0;
	container.deserialize(0, len);
	unsigned char* data = new unsigned char[len];
	if(container.deserialize((char*)data, len))
	{
		m_monitor->SendPacket(data, len);
	}
	delete data;
}
