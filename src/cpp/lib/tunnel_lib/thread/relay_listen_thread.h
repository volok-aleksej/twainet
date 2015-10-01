#ifndef RELAY_LISTEN_THREAD_H
#define RELAY_LISTEN_THREAD_H

#include <map>
#include "tunnel_lib/thread/tunnel_server_listen_address.h"
#include "thread_lib/thread/thread_impl.h"
#include "thread_lib/common/semafor.h"
#include "connector_lib/signal/signal_owner.h"
#include "connector_lib/signal/signal_receiver.h"
#include "connector_lib/message/connector_messages.h"

class ListenThread;

class RelayListenThread : public SignalOwner, public SignalReceiver, public ThreadImpl
{
public:
	RelayListenThread(const TunnelServerListenAddress& address);
	virtual ~RelayListenThread();

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
	void onAddConnector(const ConnectorMessage& msg);
private:
	TunnelServerListenAddress m_address;
	ListenThread* m_listenThreadOne;
	ListenThread* m_listenThreadTwo;
	Connector* m_connectorOne;
	Connector* m_connectorTwo;
	std::map<std::string, Address> m_addresses;
	CriticalSection m_cs;
	Semaphore m_addrSemafor;
	Semaphore m_connectSemafor;
};

#endif/*RELAY_LISTEN_THREAD_H*/