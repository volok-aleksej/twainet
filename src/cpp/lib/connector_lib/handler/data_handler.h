#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include "include/data_message.h"

class DataHandler
{
public:
	virtual bool onData(char* data, int len) = 0;
	virtual bool toMessage(const DataMessage& msg) = 0;
protected:
	DataHandler(void){}
	virtual ~DataHandler(void){}
};


#endif	// DATA_HANDLER_H
