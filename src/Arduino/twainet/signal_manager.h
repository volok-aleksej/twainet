#ifndef SIGNAL_MANAGER_H
#define SIGNAL_MANAGER_H

#include "managers_container.h"
#include "include/singleton.h"
#include "std/list.hpp"

class Signal;

class SignalManager : public Singleton<SignalManager>, public IManager
{
protected:
	friend class Singleton<SignalManager>;
	SignalManager();
	~SignalManager();
public:
	void AddSignal(Signal* signal);
protected:
    virtual void ManagerFunc();
    virtual void ManagerStart(){}
    virtual void ManagerStop(){}
    virtual bool IsStop() { return false; }
    virtual bool IsDestroyable() { return false; }
private:
	twnstd::list<Signal*> m_signals;
};

#endif/*SIGNAL_MANAGER_H*/