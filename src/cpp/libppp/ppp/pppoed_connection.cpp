#include "pppoed_connection.h"
#include "common\ref.h"
#include "application\ethernet_monitor.h"
#include "application\application.h"

std::string RandString(int size);

PPPoEDConnection::PPPoEDConnection(EthernetMonitor* monitor, const std::string& hostId)
	: m_hostId(hostId), m_sessionId(0), m_state(PADI), m_monitor(monitor)
{
	ManagersContainer::GetInstance().AddManager(static_cast<IManager*>(this));
}

PPPoEDConnection::~PPPoEDConnection()
{
}

std::string PPPoEDConnection::GetHostId() const
{
	return m_hostId;
}

bool PPPoEDConnection::operator == (const PPPoEDConnection& addr) const
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

bool PPPoEDConnection::DeleteContainer(const PPPoEDContainer* pppoed)
{
	delete pppoed;
	return true;
}

bool PPPoEDConnection::GetContainer(PPPoEDContainer** const container, const PPPoEDContainer* pppoed)
{
	if(*container)
		return false;
	
	*const_cast<PPPoEDContainer**>(container) = const_cast<PPPoEDContainer*>(pppoed);
	return true;
}

void PPPoEDConnection::OnPacket(const PPPoEDContainer& pppoed)
{
	m_containers.AddObject(new PPPoEDContainer(pppoed));
}

void PPPoEDConnection::ManagerFunc()
{
	PPPoEDContainer* container = 0;
	m_containers.CheckObjects(Ref(this, &PPPoEDConnection::GetContainer, &container));
	if(!container)
	{
		return ;
	}

	m_mac = EtherNetContainer::MacToString((const char*)container->m_ethHeader.ether_shost);
	m_hostName = container->m_tags[PPPOED_AN];

	//get PADI request
	if(container->m_pppoeHeader.code == PPPOE_PADI && m_state == PADI)
	{
		PPPoEDContainer pado(m_monitor->GetMac(), m_mac, PPPOE_PADO);
		pado.m_tags[PPPOED_ACC] = m_hostCookie = RandString(16);
		SendPPPoED(pado);
		m_state = PADO;
	}

	//get PADO request
	if(container->m_pppoeHeader.code == PPPOE_PADO)
	{
		if (m_state == PADI || 
			m_state == PADO && container->m_tags[PPPOED_HU] < Application::GetInstance().GetOwnId())
		{
			PPPoEDContainer padr(m_monitor->GetMac(), m_mac, PPPOE_PADR);
			padr.m_tags[PPPOED_ACC] = m_hostCookie = container->m_tags[PPPOED_ACC];
			SendPPPoED(padr);
			m_state = PADR;
		}
	}

	//get PADR request
	if(container->m_pppoeHeader.code == PPPOE_PADR)
	{
		switch(m_state)
		{
			case PADO:
			{
				if(container->m_tags[PPPOED_ACC] != m_hostCookie)
				{
					m_monitor->RemoveContact(m_hostId);
				}

				PPPoEDContainer pads(m_monitor->GetMac(), m_mac, PPPOE_PADS);
				pads.m_tags[PPPOED_ACC] = m_hostCookie;
				pads.m_pppoeHeader.sessionId = m_sessionId = GetTickCount();
				SendPPPoED(pads);
				m_state = PADS;
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
		switch(m_state)
		{
			case PADR:
			{
				if(container->m_tags[PPPOED_ACC] != m_hostCookie)
				{
					m_monitor->RemoveContact(m_hostId);
				}

				m_sessionId = container->m_pppoeHeader.sessionId;
				m_state = PADS;
				break;
			}
			case PADI:
			{
				m_monitor->RemoveContact(m_hostId);
				break;
			}
		}
	}

	delete container;
}

void PPPoEDConnection::ManagerStart()
{
}

void PPPoEDConnection::ManagerStop()
{
	m_containers.CheckObjects(Ref(this, &PPPoEDConnection::DeleteContainer));
}

void PPPoEDConnection::SendPPPoED(PPPoEDContainer container)
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
