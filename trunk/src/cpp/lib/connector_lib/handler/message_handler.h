#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "data_handler.h"
#include "data_message.h"

#include <map>
#include <string>

class MessageHandler
	: public DataHandler
	, virtual public Sender
{
protected:
	virtual bool onData(char* data, int len);
	virtual bool toMessage(const DataMessage& msg);
	
protected:
	MessageHandler(){}
	virtual ~MessageHandler(void);

	void addMessage(DataMessage* msg);
	bool deserialize(const DataMessage& msg, char* data, int& len);

private:
	std::map<std::string, DataMessage*> m_messages;
};

#endif	// MESSAGE_HANDLER_H
