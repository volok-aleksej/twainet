#ifndef SIGNAL_OWNER_H
#define SIGNAL_OWNER_H

#include "connector_lib/handler/data_message.h"
#include "connector_lib/signal/signal_receiver.h"

class SignalOwner
{
public:
	SignalOwner();
	virtual ~SignalOwner();

	void addSubscriber(SignalReceiver* receiver, IReceiverFunc* func);
protected:
	void onSignal(const DataMessage& msg);

private:
	Signal* m_signal;
};

#endif/*SIGNAL_OWNER_H*/