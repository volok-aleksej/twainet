#ifndef PING_THREAD_H
#define PING_THREAD_H

#include "thread_lib/thread/thread_impl.h"

class IPCConnector;

class PingThread: public ThreadImpl
{
public:
	PingThread(IPCConnector* connector);
	virtual ~PingThread();

	virtual void Stop();
protected:
	virtual void OnStart(){}
	virtual void OnStop(){}
	virtual void ThreadFunc();

private:
	IPCConnector* m_connector;
};

#endif/*PING_THREAD_H*/