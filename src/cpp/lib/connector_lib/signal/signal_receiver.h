#ifndef SIGNAL_RECEIVER_H
#define SIGNAL_RECEIVER_H

#include "signal.h"
#include "receiver_func.h"
#include "include/object_manager.h"

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
	ObjectManager<Signal*> m_signals;
	CriticalSection m_cs;
	bool m_isRemove;
};

#endif	// SIGNAL_RECEIVER_H
