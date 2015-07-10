#include "critical_section.h"

CriticalSection::CriticalSection()
{
#ifdef WIN32
	InitializeCriticalSection(&m_cs);
#else
	pthread_mutex_init(&m_cs, 0);
	m_threadlock = 0;
#endif
}

CriticalSection::~CriticalSection()
{
#ifdef WIN32
	DeleteCriticalSection(&m_cs);
#else
	pthread_mutex_destroy (&m_cs);
	m_threadlock = 0;
#endif
}

void CriticalSection::Enter()
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

void CriticalSection::Leave()
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
	{
	    m_cs.__data.__count--;
	}
#endif
}

CSLocker::CSLocker(CriticalSection* cs)
: m_cs(cs)
{
	m_cs->Enter();
}

CSLocker::~CSLocker()
{
	m_cs->Leave();
}