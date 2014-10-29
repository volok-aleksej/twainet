#ifndef APPLICATION_H
#define APPLICATION_H

#include "thread_lib\common\thread_singleton.h"
#include "twainet.h"

class Application : public ThreadSingleton<Application>
{
protected:
	template<class Object> friend class Singleton;
	Application();
	~Application();
	virtual void ThreadFunc();
	virtual void OnStop();
	virtual void OnStart();
public:
	virtual void Stop();

	void Init(const Twainet::TwainetCallback& callback);

private:
	Twainet::TwainetCallback m_callbacks;
	bool m_isExit;
};

#endif/*APPLICATION_H*/