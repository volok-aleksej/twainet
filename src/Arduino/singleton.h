#ifndef SINGLETON_H
#define SINGLETON_H

template<typename Object>
class Singleton
{
public:
	static Object& GetInstance()
	{
		static Object object;
		return object;
	}
	virtual ~Singleton(){}
protected:
	Singleton(){}
};

#endif/*SINGLETON_H*/