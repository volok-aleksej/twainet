#ifndef THREAD_H
#define THREAD_H

class Thread
{
public:
    Thread();
    virtual ~Thread();
    
    friend class ThreadManager;
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
};

#endif/*THREAD_H*/