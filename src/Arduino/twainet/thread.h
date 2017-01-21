#ifndef THREAD_H
#define THREAD_H

class Thread
{
public:
    Thread(bool destroyable);
    virtual ~Thread();
    
    friend class ThreadManager;
	bool StartThread();
    bool IsDestroyable() const;
    
	bool IsRunning() const;
    bool IsSuspend() const;
    bool IsWaiting() const;
	bool IsStopped() const;
	bool IsStop() const;
    void StopThread();

	static void sleep(unsigned long millisec);
    static void ThreadFunc(Thread*);

protected:
	virtual void ThreadFunc() = 0;
	virtual void OnStop() = 0;
	virtual void OnStart() = 0;
	virtual void Stop() = 0;

private:
    unsigned int m_threadId;
    bool m_destroyable;
};

#endif/*THREAD_H*/