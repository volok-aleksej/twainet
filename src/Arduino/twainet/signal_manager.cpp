#include "signal_manager.h"
#include "signal.h"
#include "thread_manager.h"

#include <Arduino.h>

SignalManager::SignalManager()
{
    ManagersContainer::GetInstance().AddManager(this);
}

SignalManager::~SignalManager()
{
}

void SignalManager::AddSignal(Signal* signal)
{
	m_signals.insert(m_signals.begin(), signal);
}

void SignalManager::ManagerFunc()
{
    for(twnstd::list<Signal*>::iterator it = m_signals.begin();
        it != m_signals.end(); ++it)
    {
        if((*it)->CheckSignal())
        {
            delete *it;
            it = m_signals.erase(it);
        }
    }
    
    ThreadManager::GetInstance().SwitchThread();
}