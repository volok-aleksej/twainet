#include "ipc_checker_thread.h"
#include "connector/ipc_connector.h"

IPCCheckerThread::IPCCheckerThread(IPCConnector* connector)
: m_connector(connector)
{
}

IPCCheckerThread::~IPCCheckerThread()
{
}

void IPCCheckerThread::ThreadFunc()
{
	int count = 50;
	while(!IsStop() && --count > 0)
	{
		sleep(100);
	}

	if(!count)
	{
		m_connector->Stop();
	}
}

void IPCCheckerThread::Stop()
{
	StopThread();
}

void IPCCheckerThread::OnStart()
{

}

void IPCCheckerThread::OnStop()
{

}
