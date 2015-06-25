#include "parser_states.h"
#include "application\ethernet_monitor.h"

BasicState::BasicState(EthernetMonitor* monitor)
	: m_nextState(0), m_monitor(monitor){}
BasicState::~BasicState()
{
	if(m_nextState)
	{
		delete m_nextState;
	}
}

BasicState* BasicState::NextState(char* data, int len)
{
	EtherNetContainer container;
	container.serialize(data, len);
	switch(container.m_ethHeader.ether_type)
	{
	case ETHERTYPE_PPPOED:
		m_nextState = new PPPOEDState(m_monitor);
		return m_nextState;
	default:
		return 0;
	}
}

PPPOEDState::PPPOEDState(EthernetMonitor* monitor)
	: BasicState(monitor){}
PPPOEDState::~PPPOEDState(){}

BasicState* PPPOEDState::NextState(char* data, int len)
{
	PPPoEDContainer container;
	container.serialize(data, len);
	OnPacket(container);
	return 0;
}
