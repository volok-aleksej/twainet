#include "critical_section.h"

CriticalSection::CriticalSection()
{
#ifdef WIN32
	InitializeCriticalSection(&m_cs);
#else
	pthread_mutex_init(&m_cs, 0);
#endif
}

CriticalSection::~CriticalSection()
{
#ifdef WIN32
	DeleteCriticalSection(&m_cs);
#else
	pthread_mutex_destroy (&m_cs);
#endif
}

void CriticalSection::Enter()
{
#ifdef WIN32
	EnterCriticalSection(&m_cs);
#else
	pthread_mutex_lock (&m_cs);
#endif
}

void CriticalSection::Leave()
{
#ifdef WIN32
	LeaveCriticalSection(&m_cs);
#else
	pthread_mutex_unlock(&m_cs);
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