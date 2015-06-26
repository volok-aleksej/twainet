#include "ipc_checker_thread.h"
#include "connector/ipc_connector.h"

IPCCheckerThread::IPCCheckerThread(IPCConnector* connector)
: m_connector(connector)
{
	ManagersContainer::GetInstance().AddManager(static_cast<IManager*>(this));
}

IPCCheckerThread::~IPCCheckerThread()
{
}

void IPCCheckerThread::ManagerFunc()
{
	int count = 50;
	if(--count > 0)
	{
	}

	if(!count)
	{
		m_connector->Stop();
	}
}
