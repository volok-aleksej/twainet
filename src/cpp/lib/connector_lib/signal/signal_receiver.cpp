#include "signal_receiver.h"
#include "include/ref.h"

SignalReceiver::SignalReceiver(void)
: m_isRemove(false)
{
}

SignalReceiver::~SignalReceiver(void)
{
	removeReceiver();
}

void SignalReceiver::removeSignal(Signal* signal)
{
	CSLocker locker(&m_cs);
	if(!m_isRemove)
	{
		m_signals.RemoveObject(signal);
		signal->unsubscribe(this);
	}
}

void SignalReceiver::addSignal(Signal* signal, IReceiverFunc* func)
{
	CSLocker locker(&m_cs);
	if(!m_isRemove)
	{
		m_signals.AddObject(signal);
		signal->subscribe(func);
	}
	else
	{
		delete func;
	}
}

bool SignalReceiver::onRemoveSignal(Signal* signal)
{
	CSLocker locker(&m_cs);
	if(!m_isRemove)
	{
		m_signals.RemoveObject(signal);
		return true;
	}

	return false;
}

void SignalReceiver::removeReceiver()
{
	{
		CSLocker locker(&m_cs);
		m_isRemove = true;
	}

	m_signals.CheckObjects(Ref(this, &SignalReceiver::removeSignals));
}

bool SignalReceiver::removeSignals(const Signal* signal)
{
	const_cast<Signal*>(signal)->unsubscribe(this);
	return true;
}

