#ifndef IPC_CHECKER_THREAD_H
#define IPC_CHECKER_THREAD_H

#include "thread.h"

class IPCConnector;

class IPCCheckerThread : public ThreadImpl
{
public:
	IPCCheckerThread(IPCConnector* connector);
	virtual ~IPCCheckerThread();
protected:
	virtual void ThreadFunc();
	virtual void Stop(){};
private:
	IPCConnector* m_connector;
	int m_count;
};

#endif/*IPC_CHECKER_THREAD_H*/