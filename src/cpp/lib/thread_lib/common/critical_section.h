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
	CriticalSection();
	~CriticalSection();

	void Enter();
	void Leave();
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
	CSLocker(CriticalSection* cs);
	~CSLocker();
private:
	CriticalSection* m_cs;
};

#endif/*CRITICAL_SECTION_H*/