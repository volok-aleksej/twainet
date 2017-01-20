#include "signal_manager.h"
#include "signal.h"

#include <Arduino.h>

SignalManager::SignalManager()
: Thread(false)
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
    while(true) {            
        for(twnstd::list<Signal*>::iterator it = m_signals.begin();
            it != m_signals.end(); ++it)
        {
            if((*it)->CheckSignal())
            {
                delete *it;
                it = m_signals.erase(it);
            }
        }
        SuspendThread();
    }
}