#include "signal_receiver.h"

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
	if(!m_isRemove)
	{
		for(twnstd::list<Signal*>::iterator it = m_signals.begin();
		    it != m_signals.end(); ++it)
		{
			if(*it == signal)
			{
				m_signals.erase(it);
				break;
			}
		}
		signal->unsubscribe(this);
	}
}

void SignalReceiver::addSignal(Signal* signal, IReceiverFunc* func)
{
	if(!m_isRemove)
	{
		m_signals.insert(m_signals.end(), signal);
		signal->subscribe(func);
	}
	else
	{
		delete func;
	}
}

bool SignalReceiver::onRemoveSignal(Signal* signal)
{
	if(!m_isRemove)
	{
		for(twnstd::list<Signal*>::iterator it = m_signals.begin();
		    it != m_signals.end(); ++it)
		{
			if(*it == signal)
			{
				m_signals.erase(it);
				break;
			}
		}
		return true;
	}

	return false;
}

void SignalReceiver::removeReceiver()
{
	m_isRemove = true;
	for(twnstd::list<Signal*>::iterator it = m_signals.begin();
	    it != m_signals.end(); ++it)
	{
		(*it)->unsubscribe(this);
	}
}
