#ifndef PING_THREAD_H
#define PING_THREAD_H

#include "thread_lib/common/managers_container.h"

class IPCConnector;

class PingThread: public IManager
{
public:
	PingThread(IPCConnector* connector);
	virtual ~PingThread();

protected:
	virtual void ManagerStart(){}
	virtual void ManagerStop(){}
	virtual void ManagerFunc();

private:
	IPCConnector* m_connector;
};

#endif/*PING_THREAD_H*/