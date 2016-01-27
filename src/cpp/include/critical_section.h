#ifndef CRITICAL_SECTION_H
#define CRITICAL_SECTION_H

#ifdef WIN32
#	define WIN32_LEAN_AND_MEAN
#	include <winsock2.h>
#	include <windows.h>
#else
#	include <pthread.h>
#	include <sys/types.h>
#endif/*WIN32*/

class CriticalSection
{
public:
	CriticalSection()
    {
#ifdef WIN32
        InitializeCriticalSection(&m_cs);
#else
        pthread_mutex_init(&m_cs, 0);
        m_threadlock = 0;
#endif
    }
	~CriticalSection()
    {
#ifdef WIN32
        DeleteCriticalSection(&m_cs);
#else
        pthread_mutex_destroy (&m_cs);
        m_threadlock = 0;
#endif
    }

	void Enter()
    {
#ifdef WIN32
        EnterCriticalSection(&m_cs);
#else
        if(m_threadlock != pthread_self())
        {
            pthread_mutex_lock (&m_cs);
            m_threadlock = pthread_self();
        }
        else
            m_cs.__data.__count++;
#endif
    }
    
	void Leave()
    {
#ifdef WIN32
        LeaveCriticalSection(&m_cs);
#else
        if(m_threadlock == pthread_self() && 
           m_cs.__data.__count == 0)
        {
            m_threadlock = 0;
            pthread_mutex_unlock(&m_cs);
        }
        else
            m_cs.__data.__count--;
#endif
    }
private:
#ifdef WIN32
	CRITICAL_SECTION m_cs;
#else
	pthread_mutex_t m_cs;
	pthread_t m_threadlock;
#endif/*WIN32*/
};

class CSLocker
{
public:
	CSLocker(CriticalSection* cs)
    : m_cs(cs)
    {
        m_cs->Enter();
    }
    
	~CSLocker()
    {
        m_cs->Leave();
    }
private:
	CriticalSection* m_cs;
};

#endif/*CRITICAL_SECTION_H*/