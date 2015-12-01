#ifndef DEAMON_MESSAGE_H
#define DEAMON_MESSAGE_H

#include <vector>
#include "connector_lib/handler/data_message.h"
#include "twainet/application/twainet.h"

class MessageAttr
{
public:
	MessageAttr(){}
	virtual ~MessageAttr(){}
	void SetPath(const Twainet::ModuleName& path)
	{
		m_path = path;
	}
	
	Twainet::ModuleName GetPath()
	{
		return m_path;
	}
private:
	Twainet::ModuleName m_path;
};

template<class TMessage, class THandler>
class DeamonMessage : public DataMessage, public MessageAttr, public TMessage
{
public:
  	DeamonMessage(THandler* handler)
		: m_handler(handler){}
	DeamonMessage(THandler* handler, const TMessage& msg)
		: m_handler(handler)
	{
		TMessage::operator = (msg);
	}
		
	virtual void onMessage()
	{
		TMessage* msg = static_cast<TMessage*>(this);
		m_handler->onMessage(*msg, GetPath());
	}
	
	virtual bool serialize(char* data, int len)
	{
		TMessage::Clear();
		return TMessage::ParseFromArray(data, len);
	}

	virtual bool deserialize(char* data, int& len)
	{
		int size = this->ByteSize();
		if (size > len)
		{
			len = size;
			return false;
		}

		len = size;
		return this->SerializeToArray(data, len);
	}

	virtual std::string GetName()const
	{
		return this->GetTypeName();
	}

	static std::string GetMessageName()
	{
		TMessage msg;
		return msg.GetTypeName();
	}
private:
	THandler* m_handler;
};

#endif/*DEAMON_MESSAGE_H*/