#ifndef TIMER_H
#define TIMER_H

#ifdef WIN32
#else
#	include <time.h>
#	include <signal.h>
#endif/*WIN32*/

#include <map>
#include "managers_container.h"

struct ITimerProc
{
	virtual void OnTimer() = 0;
};

class Timer
{
public:
	Timer();
	explicit Timer(int ms, ITimerProc* proc);
	Timer(const Timer& timer);
	~Timer();
	
	void operator = (const Timer& timer);
	bool operator == (const Timer& timer);
	
	ITimerProc* GetTimerProc();
	int GetId();
	int Create(int ms);
	void Destroy();
private:
#ifdef WIN32
#else
	timer_t t_id;
#endif/*WIN32*/
	int m_id;
	ITimerProc* m_proc;
};

class TimerManager : public ManagerCreator<TimerManager>
{
protected:
	friend class ManagerCreator<TimerManager>;
	TimerManager();
	~TimerManager();
public:
	int CreateTimer(int ms, ITimerProc* proc);
	void DeleteTimer(int id);
	Timer GetTimer(int id);
protected:
	friend class Timer;
	void AddTimer(int id, Timer timer);
	void RemoveTimer(int id);
	int GetNextTimerId();
protected:
	virtual void ManagerStop();
private:
	CriticalSection m_cs;
	std::map<int, Timer> m_timers;
};

#endif/*TIMER_H*/