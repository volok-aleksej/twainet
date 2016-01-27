#ifndef DESERIALIZER_H
#define DESERIALIZER_H

class Deserializer
{
public:
	virtual bool deserialize(char* data, int& len) = 0;
};

#endif	// DESERIALIZER_H
