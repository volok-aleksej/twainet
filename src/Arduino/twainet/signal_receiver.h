#ifndef SIGNAL_RECEIVER_H
#define SIGNAL_RECEIVER_H

#include "signal.h"
#include "include/receiver_func.h"
#include "std/list.hpp"

#define SIGNAL_FUNC(ClassObject, Class, MessageName, Func)\
	new ReceiverFunc<Class>(ClassObject, MessageName::GetMessageName(), reinterpret_cast<ReceiverFunc<Class>::SignalFunction>(\
							static_cast<void (Class::*)(const MessageName& msg)>(&Class::Func)))

class SignalReceiver
{
	friend class Signal;
public:
	SignalReceiver(void);
	virtual ~SignalReceiver(void);

	//Removing receiver as subscriber.
	//After call this method object of this class will never subscribe on signals.
	//Let's call on to delete object.
	void removeReceiver();

	void removeSignal(Signal* signal);
	void addSignal(Signal* signal, IReceiverFunc* func);
protected:
	bool removeSignals(const Signal* signal);
	bool onRemoveSignal(Signal* signal);
private:
	twnstd::list<Signal*> m_signals;
	bool m_isRemove;
};

#endif	// SIGNAL_RECEIVER_H
