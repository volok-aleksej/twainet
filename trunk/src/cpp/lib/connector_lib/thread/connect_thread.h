#ifndef CONNECT_THREAD_H
#define CONNECT_THREAD_H

#include "../thread/address.h"
#include "thread_lib/thread/thread_impl.h"
#include "../signal/signal.h"
#include "../signal/signal_owner.h"

class AnySocket;

class ConnectThread : public SignalOwner, public ThreadImpl
{
public:
	ConnectThread(const ConnectAddress& address);
	~ConnectThread();

	virtual void Stop();
protected:
	virtual void ThreadFunc();

protected:
	ConnectAddress m_address;
	AnySocket* m_socket;
};

#endif/*CONNECT_THREAD_H*/