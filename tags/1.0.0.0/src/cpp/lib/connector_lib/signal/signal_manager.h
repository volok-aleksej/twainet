#ifndef SIGNAL_MANAGER_H
#define SIGNAL_MANAGER_H

#include "thread_lib/common/thread_singleton.h"
#include "thread_lib/common/object_manager.h"

class Signal;

class SignalManager : public ThreadSingleton<SignalManager>
{
protected:
	template<class Object> friend class Singleton;
	SignalManager();
	~SignalManager();
public:
	void AddSignal(Signal* signal);
protected:
	template<typename TClass, typename TFunc, typename TObject> friend class ReferenceObject;
	bool CheckSignal(const std::vector<Signal*>& signals, const Signal* signal);
protected:
	virtual void ThreadFunc();
	virtual void OnStop();
	virtual void OnStart();
	virtual void Stop();
private:
	ObjectManager<Signal*> m_signals;
	bool m_isExit;
};

#endif/*SIGNAL_MANAGER_H*/