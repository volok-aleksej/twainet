#include "signal_manager.h"
#include "signal.h"

SignalManager::SignalManager()
{
    StartThread();
}

SignalManager::~SignalManager()
{
}

void SignalManager::AddSignal(Signal* signal)
{
	m_signals.insert(m_signals.begin(), signal);
}

void SignalManager::ThreadFunc()
{
	for(twnstd::list<Signal*>::iterator it = m_signals.begin();
	    it != m_signals.end(); ++it)
	{
		if((*it)->CheckSignal())
		{
			delete *it;
			it = m_signals.erase(it);
		}
		SuspendThread();
	}
}