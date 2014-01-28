#ifndef LISTEN_THREAD_H
#define LISTEN_THREAD_H

#include "address.h"
#include "thread_lib/thread/thread_impl.h"
#include "../signal/signal.h"
#include "../signal/signal_owner.h"

class AnySocket;

class ListenThread : public SignalOwner, public ThreadImpl
{
public:
	ListenThread(const ListenAddress& address);
	virtual ~ListenThread();

	virtual void Stop();
protected:
	virtual void ThreadFunc();

private:
	ListenAddress m_address;
	AnySocket* m_socket;
};

#endif/*LISTEN_THREAD_H*/