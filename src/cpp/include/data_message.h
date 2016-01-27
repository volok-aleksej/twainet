#ifndef DATA_MESSAGE_H
#define DATA_MESSAGE_H

#include "serializer.h"
#include "deserializer.h"

#include <string>
#include <typeinfo>

class DataMessage
	: public Serializer
	, public Deserializer
{
public:
	virtual ~DataMessage(){};

	virtual void onMessage(){};
	virtual bool serialize(char* data, int len){return false;}
	virtual bool deserialize(char* data, int& len){return false;}
	virtual std::string GetName()const{return typeid(*this).name();}
	virtual std::string GetDeserializeName()const{return GetName();}
};

#endif	// DATA_MESSAGE_H
