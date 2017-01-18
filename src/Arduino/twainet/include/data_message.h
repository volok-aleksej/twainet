#ifndef DATA_MESSAGE_H
#define DATA_MESSAGE_H

class Deserializer
{
public:
    virtual bool deserialize(char* data, int& len) = 0;
};

class Serializer
{
public:
    virtual bool serialize(char* data, int len) = 0;
};

class DataMessage
	: public Serializer
	, public Deserializer
{
public:
	virtual ~DataMessage(){};

	virtual void onMessage(){};
	virtual bool serialize(char* data, int len){return false;}
	virtual bool deserialize(char* data, int& len){return false;}
	virtual char* GetName()const = 0;
	virtual char* GetDeserializeName()const{return GetName();}
};

#endif	// DATA_MESSAGE_H
