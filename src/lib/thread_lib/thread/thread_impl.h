#ifndef THREAD_IMPL_H
#define THREAD_IMPL_H

#include "thread.h"

class ThreadImpl : public Thread
{
public:
	ThreadImpl();
	virtual ~ThreadImpl();
protected:
	virtual void OnStop();
	virtual void OnStart();
};

#endif/*THREAD_IMPL_H*/