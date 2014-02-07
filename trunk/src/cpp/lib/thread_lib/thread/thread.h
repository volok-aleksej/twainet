#ifndef THREAD_H
#define THREAD_H

#include "../common/critical_section.h"

#ifdef WIN32
#define _WINSOCKAPI_   // prevent inclusion of winsock.h, we need winsock2.h
#include <windows.h>
#else
#include <pthread.h>
#endif/*WIN32*/

class Thread
{
public:
	enum ThreadState
	{
		THREAD_STOPPED = 0,
		THREAD_RUNNING,
		THREAD_STOP_PENDING,
		THREAD_START_PENDING
	};
private:
	template<class Object> friend class ThreadSingleton;
	friend class ThreadImpl;
	Thread();
	virtual ~Thread();

protected:
	friend class ThreadManager;
	void StopThread();

public:
	bool Start();
	void Join();
	bool IsRunning();
	bool IsStopped() const;
	bool IsStop();

	static void sleep(unsigned long millisec);

public:
#ifdef WIN32
    static unsigned int __stdcall ThreadProc(void* arg);
#else
    static void* ThreadProc(void* arg);
#endif

	virtual void ThreadFunc() = 0;
	virtual void OnStop() = 0;
	virtual void OnStart() = 0;
	virtual void Stop() = 0;

private:
#ifdef WIN32
    typedef HANDLE ThreadHandle;
    unsigned int m_threadId;
#else
    typedef pthread_t ThreadHandle;
#endif
    ThreadHandle m_handle;
    ThreadState m_state;
	CriticalSection m_cs;
};

#endif/*THREAD_H*/