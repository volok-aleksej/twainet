#ifndef SIGNAL_MANAGER_H
#define SIGNAL_MANAGER_H

#include "thread_lib/common/managers_container.h"
#include "thread_lib/common/object_manager.h"

class Signal;

class SignalManager : public ManagerCreator<SignalManager>
{
protected:
	friend class ManagerCreator<SignalManager>;
	SignalManager();
	~SignalManager();
public:
	void AddSignal(Signal* signal);
protected:
	template<typename TClass, typename TFunc, typename TObject, typename TReturn> friend class ReferenceObject;
	bool CheckSignal(const std::vector<Signal*>& signals, const Signal* signal);
protected:
	virtual void ManagerFunc();
private:
	ObjectManager<Signal*> m_signals;
};

#endif/*SIGNAL_MANAGER_H*/