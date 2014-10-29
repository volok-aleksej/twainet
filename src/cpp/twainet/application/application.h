#ifndef APPLICATION_H
#define APPLICATION_H

#include "common\singleton.h"
#include "twainet.h"

class Application : public Singleton<Application>
{
protected:
	template<class Object> friend class Singleton;
	Application();
	~Application();
public:
	void Init(const Twainet::TwainetCallback& callback);

private:
	Twainet::TwainetCallback m_callbacks;
};

#endif/*APPLICATION_H*/