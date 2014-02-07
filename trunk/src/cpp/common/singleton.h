#ifndef SINGLTON_H
#define SINGLTON_H

template<class Object>
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

#endif/*SINGLTON_H*/