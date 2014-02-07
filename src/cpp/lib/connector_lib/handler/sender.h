#ifndef SENDER_H
#define SENDER_H

class Sender
{
public:
	virtual ~Sender(){}
	virtual bool SendData(char* data, int len) = 0;
};

#endif/*SENDER_H*/