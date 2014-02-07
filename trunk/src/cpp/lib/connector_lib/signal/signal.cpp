#include "signal.h"
#include "signal_receiver.h"
#include "receiver_func.h"
#include "common\ref.h"
#include "thread_lib/thread/thread_manager.h"
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
	m_receiverFunctions.ProcessingObjects(Ref(this, &Signal::onSignalFunc, msg));
}

void Signal::subscribe(IReceiverFunc* obj)
{
	m_receiverFunctions.AddObject(obj);
}

void Signal::unsubscribe(SignalReceiver* receiver)
{
	m_receiverFunctions.CheckObjects(Ref(this, &Signal::unsubsribeFunc, receiver));
}

void Signal::removeOwner()
{
	m_owner = 0;
	m_receiverFunctions.CheckObjects(Ref(this, &Signal::unsubsribeReceiver));
}

void Signal::onSignalFunc(const DataMessage& msg, const IReceiverFunc* obj)
{
	if(const_cast<IReceiverFunc*>(obj)->isSignal(msg))
	{
		const_cast<IReceiverFunc*>(obj)->onSignal(msg);
	}
}

bool Signal::unsubsribeFunc(const SignalReceiver* receiver, const IReceiverFunc* signal)
{
	if(const_cast<IReceiverFunc*>(signal)->GetReciever() == receiver)
	{
		delete signal;
		return true;
	}

	return false;
}

bool Signal::unsubsribeReceiver(const IReceiverFunc* signal)
{
	bool res = const_cast<IReceiverFunc*>(signal)->GetReciever()->onRemoveSignal(this);
	if(res)
	{
		delete signal;
	}
	return res;
}

bool Signal::CheckSignal()
{
	return !m_owner && !m_receiverFunctions.GetObjectList().size();
}