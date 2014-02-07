#ifndef SIGNAL_MESSAGE_H
#define SIGNAL_MESSAGE_H

#include "connector_lib/handler/data_message.h"

template<class TMessage>
class SignalMessage : public DataMessage, public TMessage
{
public:
	SignalMessage(const TMessage& msg)
	{
		TMessage::operator = (msg);
	}

	static std::string GetMessageName()
	{
		return typeid(SignalMessage<TMessage>).name();
	}
};

#endif/*SIGNAL_MESSAGE_H*/