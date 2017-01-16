#include "signal.h"
#include "signal_receiver.h"
#include "signal_manager.h"

Signal::Signal(SignalOwner* owner)
: m_owner(owner)
{
	SignalManager::GetInstance().AddSignal(this);
}

Signal::~Signal(void)
{
}

void Signal::onSignal(const DataMessage& msg)
{
	for(twnstd::list<IReceiverFunc*>::iterator it = m_receiverFunctions.begin();
	    it != m_receiverFunctions.end(); ++it)
	{
		onSignalFunc(msg, *it);
	}
}

void Signal::subscribe(IReceiverFunc* obj)
{
	m_receiverFunctions.insert(m_receiverFunctions.end(), obj);
}

void Signal::unsubscribe(SignalReceiver* receiver)
{
	for(twnstd::list<IReceiverFunc*>::iterator it = m_receiverFunctions.begin();
	    it != m_receiverFunctions.end(); ++it)
	{
		if((*it)->GetReciever() == receiver) {
			delete *it;
			it = m_receiverFunctions.erase(it);
		}
	}
}

void Signal::removeOwner()
{
	m_owner = 0;
	for(twnstd::list<IReceiverFunc*>::iterator it =m_receiverFunctions.begin();
	    it != m_receiverFunctions.end(); ++it)
	{
		if((*it)->GetReciever()->onRemoveSignal(this)) {
			delete *it;
			it = m_receiverFunctions.erase(it);
		}
	}
}

bool Signal::onSignalFunc(const DataMessage& msg, const IReceiverFunc* obj)
{
	if(const_cast<IReceiverFunc*>(obj)->isSignal(msg))
	{
		const_cast<IReceiverFunc*>(obj)->onSignal(msg);
	}
	return true;
}

bool Signal::CheckSignal()
{
	return !m_owner && !m_receiverFunctions.size();
}