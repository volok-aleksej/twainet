#ifndef THREAD_SINGLETON_H
#define THREAD_SINGLETON_H

#include "include/singleton.h"
#include "../thread/thread.h"

template<class Object>
class ThreadSingleton :
	public Singleton<Object>, public Thread
{
protected:
	ThreadSingleton(){}
public:
	virtual ~ThreadSingleton(){}
};

#endif/*THREAD_SINGLETON_H*/