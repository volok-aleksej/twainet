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
	case ETHERTYPE_PPPOES:
		m_nextState = new PPPOESState(m_monitor);
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

PPPOESState::PPPOESState(EthernetMonitor* monitor)
	: BasicState(monitor){}
PPPOESState::~PPPOESState(){}

BasicState* PPPOESState::NextState(char* data, int len)
{
	PPPoESContainer container;
	container.serialize(data, len);
	switch(container.m_protocol)
	{
	case PPPOES_LCP:
		//TODO(): create next state
	default:
		return 0;
	}
}