#include "ipc_checker_thread.h"
#include "connector/ipc_connector.h"

IPCCheckerThread::IPCCheckerThread(IPCConnector* connector)
: m_connector(connector), m_count(50)
{
	ManagersContainer::GetInstance().AddManager(static_cast<IManager*>(this));
}

IPCCheckerThread::~IPCCheckerThread()
{
}

void IPCCheckerThread::ManagerFunc()
{
	if(--m_count > 0 || IsStop())
	{
	      return;
	}

	if(!m_count)
	{
		m_connector->Stop();
	}
}
