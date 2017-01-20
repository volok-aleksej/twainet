#include "ipc_checker_thread.h"
#include "ipc_connector.h"

IPCCheckerThread::IPCCheckerThread(IPCConnector* connector)
: Thread(true), m_connector(connector), m_count(50)
{
}

IPCCheckerThread::~IPCCheckerThread()
{
}

void IPCCheckerThread::ThreadFunc()
{
	if(--m_count > 0 || IsStop())
	{
	      return;
	}

	if(!m_count)
	{
		m_connector->StopThread();
	}
}
