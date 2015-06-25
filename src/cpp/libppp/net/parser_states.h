#ifndef PARSER_STATES_H
#define PARSER_STATES_H

#include <string>

class EthernetMonitor;

class BasicState
{
public:
	BasicState(EthernetMonitor* monitor);
	virtual ~BasicState();

	virtual BasicState* NextState(char* data, int len);

	template<class Container>
	void OnPacket(Container container)
	{
		m_monitor->OnPacket(container);
	}

private:
	EthernetMonitor* m_monitor;
	BasicState* m_nextState;
};

class PPPOEDState : public BasicState
{
public:
	PPPOEDState(EthernetMonitor* monitor);
	virtual ~PPPOEDState();

	virtual BasicState* NextState(char* data, int len);
};


#endif/*PARSER_STATES_H*/