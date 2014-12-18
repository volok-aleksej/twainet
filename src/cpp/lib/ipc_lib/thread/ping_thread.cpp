#include "ping_thread.h"
#include "connector/ipc_connector.h"

PingThread::PingThread(IPCConnector* connector)
: m_connector(connector)
{
}

PingThread::~PingThread()
{
}

void PingThread::Stop()
{
	StopThread();
}

void PingThread::ThreadFunc()
{
	while(!IsStop())
	{
		PingMessage msg(m_connector);
		m_connector->toMessage(msg);
		sleep(1000);
	}
}