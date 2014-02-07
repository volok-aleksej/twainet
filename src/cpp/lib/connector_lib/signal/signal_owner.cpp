#include "signal_owner.h"

SignalOwner::SignalOwner()
{
	m_signal = new Signal(this);
}

SignalOwner::~SignalOwner()
{
	m_signal->removeOwner();
}

void SignalOwner::onSignal(const DataMessage& msg)
{
	m_signal->onSignal(msg);
}

void SignalOwner::addSubscriber(SignalReceiver* receiver, IReceiverFunc* func)
{
	receiver->addSignal(m_signal, func);
}
