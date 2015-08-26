#ifndef INTERNAL_CONNECTOR_H
#define INTERNAL_CONNECTOR_H

#include "connector_lib/connector/connector.h"
#include "connector_lib/signal/signal_receiver.h"
#include "connector_lib/signal/signal_owner.h"
#include "ipc_connector.h"

class InternalConnector : public Connector, public SignalReceiver, protected SignalOwner
{
public:
	InternalConnector(AnySocket* socket);
	virtual ~InternalConnector();
	
	void SubscribeConnector(::SignalOwner* owner);
protected:
	friend class IPCConnector;
	friend class Signal;
	void onInternalConnectionDataMessage(const InternalConnectionDataMessage& msg);
protected:
	virtual void ThreadFunc();
	virtual void OnStart();
	virtual void OnStop();
private:
	int m_maxBufferSize;
};

#endif/*INTERNAL_CONNECTOR_H*/