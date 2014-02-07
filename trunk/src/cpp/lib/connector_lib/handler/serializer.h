#ifndef SERIALIZER_H
#define SERIALIZER_H

class Serializer
{
public:
	virtual bool serialize(char* data, int len) = 0;
};

#endif	// SERIALIZER_H