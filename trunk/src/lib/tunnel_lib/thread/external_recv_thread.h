#ifndef EXTERNAL_RECV_THREAD_H
#define EXTERNAL_RECV_THREAD_H

#include "thread_lib/thread/thread_impl.h"
#include "connector_lib/signal/signal_owner.h"

class UDPSocket;

class ExternalRecvThread : public SignalOwner, public ThreadImpl
{
public:
	ExternalRecvThread(UDPSocket* socket, const std::string& id);
	virtual ~ExternalRecvThread();

	virtual void Stop();
protected:
	virtual void OnStart(){}
	virtual void OnStop(){}
	virtual void ThreadFunc();
private:
	UDPSocket* m_socket;
	std::string m_id;
};

#endif/*EXTERNAL_RECV_THREAD_H*/