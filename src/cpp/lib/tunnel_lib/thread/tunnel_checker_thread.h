#ifndef TUNNEL_CHECKER_THREAD_H
#define TUNNEL_CHECKER_THREAD_H

#include "thread_lib\common\managers_container.h"

class TunnelModule;

class TunnelCheckerThread : public IManager
{
public:
	TunnelCheckerThread(TunnelModule* module);
	virtual ~TunnelCheckerThread();

protected:
	virtual void ManagerStart(){}
	virtual void ManagerStop(){}
	virtual void ManagerFunc();
private:
	TunnelModule* m_module;
};

#endif/*TUNNEL_CHECKER_THREAD_H*/