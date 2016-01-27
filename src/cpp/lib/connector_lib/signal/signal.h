#ifndef SIGNAL_H
#define SIGNAL_H

#include "include/object_manager.h"
#include "include/data_message.h"
#include "thread_lib/thread/thread_impl.h"
#include "receiver_func.h"

class SignalOwner;

class Signal
{
public:
	Signal(SignalOwner* owner);

	void onSignal(const DataMessage& msg);
	void removeOwner();
protected:
	friend class SignalManager;
	bool CheckSignal();
	virtual ~Signal(void);
protected:
	friend class SignalReceiver;
	void subscribe(IReceiverFunc* obj);
	void unsubscribe(SignalReceiver* receiver);
private:
	bool onSignalFunc(const DataMessage& msg, const IReceiverFunc* obj);
	bool unsubsribeFunc(const SignalReceiver* receiver, const IReceiverFunc* signal);
	bool unsubsribeReceiver(const IReceiverFunc* signal);
	ObjectManager<IReceiverFunc*> m_receiverFunctions;
	SignalOwner* m_owner;
};

#endif	// SIGNAL_H
