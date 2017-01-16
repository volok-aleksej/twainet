#ifndef SIGNAL_OWNER_H
#define SIGNAL_OWNER_H

#include "include/data_message.h"
#include "signal_receiver.h"

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