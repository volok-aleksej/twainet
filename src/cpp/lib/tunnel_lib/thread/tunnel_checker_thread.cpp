#include "tunnel_checker_thread.h"
#include "module/tunnel_module.h"

TunnelCheckerThread::TunnelCheckerThread(TunnelModule* module)
: m_module(module)
{
	ManagersContainer::GetInstance().AddManager(static_cast<IManager*>(this));
}

TunnelCheckerThread::~TunnelCheckerThread()
{
}

void TunnelCheckerThread::ManagerFunc()
{
	if(!IsStop())
	{
		m_module->CheckTunnels();
	}
}

