#ifndef EXTERNAL_LISTEN_THREAD_H
#define EXTERNAL_LISTEN_THREAD_H

#include <map>
#include "external_recv_thread.h"
#include "tunnel_lib/thread/tunnel_server_listen_address.h"
#include "thread_lib/thread/thread_impl.h"
#include "connector_lib/signal/signal_owner.h"
#include "connector_lib/signal/signal_receiver.h"
#include "connector_lib/message/connector_messages.h"
#include "thread_lib/common/semafor.h"

class UDPSocket;

class ExternalListenThread : public SignalOwner, public SignalReceiver, public ThreadImpl
{
public:
	ExternalListenThread(const TunnelServerListenAddress& address);
	virtual ~ExternalListenThread();

	virtual void Stop();
protected:
	virtual void OnStart(){}
	virtual void OnStop(){}
	virtual void ThreadFunc();

	void SignalError();
protected:
	friend class Signal;
	void onListenErrorMessage(const ListenErrorMessage& msg);
	void onCreatedListenerMessage(const CreatedListenerMessage& msg);
private:
	TunnelServerListenAddress m_address;
	ExternalRecvThread* m_recvThreadOne;
	ExternalRecvThread* m_recvThreadTwo;
	std::map<std::string, Address> m_addresses;
	CriticalSection m_cs;
	Semaphore m_semafor;
};

#endif/*EXTERNAL_LISTEN_THREAD_H*/