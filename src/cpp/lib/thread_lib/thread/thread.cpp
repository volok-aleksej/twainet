#include "thread.h"
#ifdef WIN32
#	include <process.h>
#endif/*WIN32*/

void Thread::sleep(unsigned long millisec)
{
#ifdef WIN32
	Sleep(millisec);
#else
	usleep(millisec*1000);
#endif
}

Thread::Thread()
: m_handle(0)
#ifdef WIN32
, m_threadId(0)
#endif
, m_state(THREAD_STOPPED)
{
}

Thread::~Thread()
{
    if(m_handle)
    {
#ifdef WIN32
    ::CloseHandle(m_handle);
#else
      pthread_detach(m_handle);
#endif
      m_handle = 0;
    }
}

bool Thread::Start()
{
	{
		CSLocker locker(&m_cs);
		if(m_state != THREAD_STOPPED)
		{
			return false;
		}
		m_state = THREAD_START_PENDING;
	}

#ifdef WIN32

    m_handle = (ThreadHandle)_beginthreadex(NULL, 0, ThreadProc, (void*)this, 0, &m_threadId);
    if (m_handle == 0)
       return false;
#else
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    int re = pthread_create(&m_handle, &attr, ThreadProc, this);
    pthread_attr_destroy(&attr);
    if(re != 0)
        return false;
#endif
	while(m_state == THREAD_START_PENDING)
	{
		sleep(100);
	}

	return true;
}

void Thread::Join()
{
	if(!m_handle || m_state == THREAD_STOPPED)
	{
		return;
	}
	else if(m_state == THREAD_START_PENDING)
	{
		while(m_state != THREAD_RUNNING)
		{
			sleep(100);
		}
	}

	StopThread();

	Stop();

	while(m_state != THREAD_STOPPED)
	{
#ifdef WIN32
		DWORD exitCode = 0;
		::GetExitCodeThread(m_handle, &exitCode);
		if(exitCode != STILL_ACTIVE)
		{
			break;
		}
#endif
		sleep(100);
	}

#ifdef WIN32
	m_threadId = 0;
#else
	pthread_join(m_handle, 0);
#endif
    m_handle = 0;
}

#ifdef WIN32
unsigned int __stdcall Thread::ThreadProc(void* arg)
#else
void* Thread::ThreadProc(void* arg)
#endif
{
	Thread* thread = (Thread*)arg;

	{
		CSLocker locker(&thread->m_cs);
		thread->m_state = THREAD_RUNNING;
	}

	thread->OnStart();
	thread->ThreadFunc();
	thread->OnStop();

	{
		CSLocker locker(&thread->m_cs);
		thread->m_state = THREAD_STOPPED;
	}

#ifdef WIN32
	_endthread();
#endif
    return 0;
}

bool Thread::IsStopped() const
{
	return m_state == THREAD_STOPPED;
}

bool Thread::IsStop()
{
	return m_state == THREAD_STOP_PENDING;
}

bool Thread::IsRunning()
{
	return m_state == THREAD_RUNNING;
}


void Thread::StopThread()
{
	CSLocker locker(&m_cs);
	if(m_state != THREAD_STOPPED)
	{
		m_state = THREAD_STOP_PENDING;
	}
}