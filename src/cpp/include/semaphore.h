#ifndef SEMAFOR_H
#define SEMAFOR_H

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <winsock2.h>
    #include <windows.h>
#else
    #include <semaphore.h>
    #include <fcntl.h>
    #include <errno.h>
    #include <time.h>
    #define INFINITE    -1
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
    Semaphore(int initial = 0, int count = 1)
    {
    #ifdef WIN32
        m_semafor = CreateSemaphore(NULL, initial, count, NULL);
    #else
        sem_init(&m_semafor, 0, initial);
    #endif/*WIN32*/
    }
    
    ~Semaphore()
    {
    #ifdef WIN32
        CloseHandle(m_semafor);
    #else
        sem_destroy(&m_semafor);
    #endif/*WIN32*/
    }
    
    void Set()
    {
    #ifdef WIN32
        ReleaseSemaphore(m_semafor, 1, NULL);
    #else
        sem_post(&m_semafor);
    #endif/*WIN32*/
    }
    
    void Release()
    {
    #ifdef WIN32
        WaitForSingleObject(m_semafor, 0);
    #else
        sem_trywait(&m_semafor);
    #endif/*WIN32*/
    }
    
    WaitStatus Wait(int timeout)
    {
    #ifdef WIN32
        DWORD ret = WaitForSingleObject(m_semafor, timeout);
        switch(ret)
        {
        case WAIT_TIMEOUT:
            return TIMEOUT;
        case WAIT_OBJECT_0:
            ReleaseSemaphore(m_semafor, 1, NULL);
            return SUCCESS;
        }
    #else
        int ret;
        if(timeout == INFINITE)
        {
            ret = sem_wait(&m_semafor);
        }
        else
        {
            struct timespec tm = {timeout, timeout*1000000};
            ret = sem_timedwait(&m_semafor, &tm);
        }
        if(ret && errno == ETIMEDOUT)
        {
            return TIMEOUT;
        }
        else if(!ret)
        {
            sem_post(&m_semafor);
            return SUCCESS;
        }
    #endif/*WIN32*/
        return FAILED;
    }
    
    bool IsSet()
    {
    #ifdef WIN32
        DWORD ret = WaitForSingleObject(m_semafor, 0);
        if(ret == WAIT_OBJECT_0)
        {
            ReleaseSemaphore(m_semafor, 1, NULL);
            return true;
        }
    #else
        int ret = sem_trywait(&m_semafor);
        if(ret == 0)
        {
            sem_post(&m_semafor);
            return true;
        }
    #endif/*WIN32*/
        return false;
    }
    
private:
#ifdef WIN32
    HANDLE m_semafor;
#else
    sem_t m_semafor;
#endif/*WIN32*/
};

#endif/*SEMAFOR_H*/