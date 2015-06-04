#include "signal_manager.h"
#include "signal.h"

#include "common/ref.h"

SignalManager::SignalManager()
{
}

SignalManager::~SignalManager()
{
}

void SignalManager::AddSignal(Signal* signal)
{
	m_signals.AddObject(signal);
}

bool SignalManager::CheckSignal(const std::vector<Signal*>& signals, const Signal* signal)
{
	Signal* signal_ = const_cast<Signal*>(signal);
	if(signal_->CheckSignal())
	{
		const_cast<std::vector<Signal*>&>(signals).push_back(signal_);
		return true;
	}

	return false;
}

void SignalManager::ManagerFunc()
{
	std::vector<Signal*> signals;
	m_signals.CheckObjects(Ref(this, &SignalManager::CheckSignal, signals));

	for(std::vector<Signal*>::iterator it = signals.begin(); it != signals.end(); it++)
	{
		delete *it;
	}
}