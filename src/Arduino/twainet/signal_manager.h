#ifndef SIGNAL_MANAGER_H
#define SIGNAL_MANAGER_H

#include "thread.h"
#include "include/singleton.h"
#include "std/list.hpp"

class Signal;

class SignalManager : public Singleton<SignalManager>, public Thread
{
protected:
	friend class Singleton<SignalManager>;
	SignalManager();
	~SignalManager();
public:
	void AddSignal(Signal* signal);
protected:
	virtual void ThreadFunc();
	virtual void Stop(){};
    virtual void OnStop(){};
    virtual void OnStart(){};
private:
	twnstd::list<Signal*> m_signals;
};

#endif/*SIGNAL_MANAGER_H*/