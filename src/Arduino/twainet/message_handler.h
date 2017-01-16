#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <WString.h>

#include "std/vector.hpp"
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

class Sender
{
public:
    virtual ~Sender(){}
    virtual bool SendData(char* data, int len) = 0;
};

class MessageHandler
	: public DataHandler
	, virtual public Sender
{
protected:
	virtual bool onData(char* data, int len);
	virtual bool toMessage(const DataMessage& msg);
	
protected:
	MessageHandler();
	virtual ~MessageHandler(void);

	void addMessage(DataMessage* msg);
	bool deserialize(const DataMessage& msg, char* data, int& len);

	bool onData(char* type, char* data, int len);
private:
	twnstd::vector<DataMessage*> m_messages;
};

#endif	// MESSAGE_HANDLER_H
