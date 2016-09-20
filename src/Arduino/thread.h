#ifndef THREAD_H
#define THREAD_H

extern unsigned int g_threadCounter;

class Thread
{
public:
	enum ThreadState
	{
        THREAD_CREATED = 0,
		THREAD_STOPPED,
		THREAD_RUNNING,
		THREAD_STOP_PENDING,
		THREAD_START_PENDING
	};
private:
    friend class ThreadManager;
	friend class ThreadImpl;
	Thread();
	virtual ~Thread();

public:
	bool StartThread();
	bool IsRunning();
	bool IsStopped() const;
	bool IsStop();
    void StopThread();

	static void sleep(unsigned long millisec);

protected:
	virtual void ThreadFunc() = 0;
	virtual void OnStop() = 0;
	virtual void OnStart() = 0;
	virtual void Stop() = 0;

private:
    unsigned int m_threadId;
    ThreadState m_state;
};

class ThreadImpl : public Thread
{
public:
    ThreadImpl();
    virtual ~ThreadImpl();
protected:
    virtual void OnStop();
    virtual void OnStart();
};

#endif/*THREAD_H*/