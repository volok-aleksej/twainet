#include "tunnel_checker_thread.h"
#include "module\tunnel_module.h"

TunnelCheckerThread::TunnelCheckerThread(TunnelModule* module)
: m_module(module)
{
	ManagersContainer::GetInstance().AddManager(static_cast<IManager*>(this));
}

TunnelCheckerThread::~TunnelCheckerThread()
{
	ManagersContainer::GetInstance().RemoveManager(static_cast<IManager*>(this));
}

void TunnelCheckerThread::ManagerFunc()
{
	m_module->CheckTunnels();
}

