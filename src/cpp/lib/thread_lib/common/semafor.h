#ifndef SEMAFOR_H
#define SEMAFOR_H

#ifdef WIN32
#include <windows.h>
#else
#include <semaphore.h>
#define INFINITE	-1
#endif/*WIN32*/


class Semaphore
{
public:
	enum WaitStatus
	{
		TIMEOUT,
		FAILED,
		SUCCESS
	};
public:
	Semaphore(int initial = 0, int count = 1);
	~Semaphore();
	
	void Set();
	void Release();
	WaitStatus Wait(int timeout);
	bool IsSet();
private:
#ifdef WIN32
	HANDLE m_semafor;
#else
	sem_t m_semafor;
#endif/*WIN32*/
};

#endif/*SEMAFOR_H*/