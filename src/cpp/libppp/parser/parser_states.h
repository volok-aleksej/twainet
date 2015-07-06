#ifndef PARSER_STATES_H
#define PARSER_STATES_H

#include <string>

class EthernetMonitor;
class IConnectionPacket;

class BasicState
{
public:
	BasicState(EthernetMonitor* monitor);
	virtual ~BasicState();

	virtual BasicState* NextState(char* data, int len);

	void OnPacket(const IConnectionPacket& container);

protected:
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

class PPPOESState : public BasicState
{
public:
	PPPOESState(EthernetMonitor* monitor);
	virtual ~PPPOESState();

	virtual BasicState* NextState(char* data, int len);
};

class PPPLCPState : public BasicState
{
public:
	PPPLCPState(EthernetMonitor* monitor);
	virtual ~PPPLCPState();

	virtual BasicState* NextState(char* data, int len);
};
#endif/*PARSER_STATES_H*/