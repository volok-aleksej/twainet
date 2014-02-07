#ifndef IPC_CHECKER_THREAD_H
#define IPC_CHECKER_THREAD_H

#include "thread_lib/thread/thread_impl.h"

class IPCConnector;

class IPCCheckerThread : public ThreadImpl
{
public:
	IPCCheckerThread(IPCConnector* connector);
	virtual ~IPCCheckerThread();

	void Stop();

protected:
	void ThreadFunc();
	void OnStart();
	void OnStop();
private:
	IPCConnector* m_connector;
};

#endif/*IPC_CHECKER_THREAD_H*/