#include "pppoed_connection.h"
#include "common\ref.h"
#include "application\ethernet_monitor.h"

std::string CreateGUID();

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
	return addr.m_hostId == m_hostId;
}

bool PPPoEDConnection::DeleteContainer(const PPPoEDContainer* pppoed)
{
	delete pppoed;
	return true;
}

bool PPPoEDConnection::GetContainer(PPPoEDContainer** const container, const PPPoEDContainer* pppoed)
{
	if(container)
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
	PPPoEDContainer* container;
	m_containers.CheckObjects(Ref(this, &PPPoEDConnection::GetContainer, &container));
	m_mac = EtherNetContainer::MacToString((const char*)container->m_ethHeader.ether_shost);
	m_hostName = container->m_tags[PPPOED_AN];
	if(container->m_pppoeHeader.code == PPPOE_PADI && m_state == PADI)
	{
		PPPoEDContainer pado(m_monitor->GetMac(), m_mac, PPPOE_PADO);
		pado.m_tags[PPPOED_ACC] = m_hostCookie = CreateGUID();
		int len = 0;
		pado.deserialize(0, len);
		unsigned char* data = new unsigned char[len];
		if(pado.deserialize((char*)data, len))
		{
			m_monitor->SendPacket(data, len);
		}
		delete data;
		m_state = PADO;
	}

	if(container->m_pppoeHeader.code == PPPOE_PADO)
	{
		if(m_state == PADI)
		{
		}
		else if(m_state == PADO)
		{
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