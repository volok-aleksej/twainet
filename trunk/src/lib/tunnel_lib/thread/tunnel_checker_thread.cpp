#include "tunnel_checker_thread.h"
#include "module\tunnel_module.h"

TunnelCheckerThread::TunnelCheckerThread(TunnelModule* module)
: m_module(module)
{
}

TunnelCheckerThread::~TunnelCheckerThread()
{

}

void TunnelCheckerThread::Stop()
{
	StopThread();
}

void TunnelCheckerThread::ThreadFunc()
{
	while (!IsStop())
	{
		m_module->CheckTunnels();
		sleep(1000);
	}
}

