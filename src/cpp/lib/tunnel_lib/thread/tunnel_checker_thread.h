#ifndef TUNNEL_CHECKER_THREAD_H
#define TUNNEL_CHECKER_THREAD_H

#include "thread_lib\thread\thread_impl.h"

class TunnelModule;

class TunnelCheckerThread : public ThreadImpl
{
public:
	TunnelCheckerThread(TunnelModule* module);
	virtual ~TunnelCheckerThread();

	virtual void Stop();
protected:
	virtual void OnStart(){}
	virtual void OnStop(){}
	virtual void ThreadFunc();
private:
	TunnelModule* m_module;
};

#endif/*TUNNEL_CHECKER_THREAD_H*/