#ifndef PROTO_MESSAGE_H
#define PROTO_MESSAGE_H

#include "include/data_message.h"

class IPCHandler;

template<typename TMessage, typename THandler = IPCHandler>
class ProtoMessage : public DataMessage, public TMessage
{
public:
	ProtoMessage(THandler* handler)
		: m_handler(handler){}
	ProtoMessage(THandler* handler, const TMessage& msg)
		: m_handler(handler)
	{
		TMessage::operator = (msg);
	}

	~ProtoMessage(){}

	virtual void onMessage()
	{
		TMessage* msg = static_cast<TMessage*>(this);
		m_handler->onMessage(*msg);
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

template<class TMessage>
class ProtoSendMessage : public DataMessage, public IPCMessage, public TMessage
{
public:
	ProtoSendMessage(){}
	ProtoSendMessage(const TMessage& message)
	{
		TMessage::operator = (message);
	}

	bool createMessage()
	{
		int size = TMessage::ByteSize();
		std::string newData;
		newData.resize(size, 0);
		set_message_name(GetName());
		if(!TMessage::SerializeToArray((char*)newData.c_str(), size))
		{
			return false;
		}
		
		set_message(newData);
		return true;
	}

	virtual std::string GetName()const
	{
		return TMessage::GetTypeName();
	}

	virtual std::string GetDeserializeName()const
	{
		return IPCMessage::GetTypeName();
	}
};

template<typename TMessage>
class SimpleProtoMessage : public DataMessage, public TMessage
{
public:
	SimpleProtoMessage(){}
	SimpleProtoMessage(const TMessage& msg, const IPCName& ipcPath)
		: m_ipcPath(ipcPath)
	{
		TMessage::operator = (msg);
	}
	virtual ~SimpleProtoMessage(){}

	virtual bool deserialize(char* data, int& len)
	{
		int size = this->ByteSize();
		std::string newData;
		newData.resize(size, 0);
		IPCMessage msg;
		msg.set_message_name(this->GetTypeName());
		*msg.add_ipc_path() = m_ipcPath;
		msg.set_message(newData);
		int newSize = msg.ByteSize();
		if (newSize > len)
		{
			len = newSize;
			return false;
		}

		len = newSize;
		if(!this->SerializeToArray((char*)newData.c_str(), size))
		{
			return false;
		}
		
		msg.set_message(newData);

		return msg.SerializeToArray(data, len);
	}

	virtual std::string GetDeserializeName()const
	{
		IPCMessage msg;
		return msg.GetTypeName();
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
protected:
	IPCName m_ipcPath;
};

template<typename THandler, typename TMessage>
class HandlerProtoMessage : public SimpleProtoMessage<TMessage>
{
public:
	HandlerProtoMessage(THandler* handler)
		: m_handler(handler){}
	HandlerProtoMessage(THandler* handler, const TMessage& msg, const IPCName& ipcPath)
		: SimpleProtoMessage<TMessage>(msg, ipcPath), m_handler(handler){}

	~HandlerProtoMessage(){}

	virtual void onMessage()
	{
		TMessage* msg = static_cast<TMessage*>(this);
		if(m_handler)
			m_handler->onMessage(*msg);
	}

	virtual bool serialize(char* data, int len)
	{
		TMessage::Clear();
		return TMessage::ParseFromArray(data, len);
	}
private:
	THandler* m_handler;
};

#endif/*PROTO_MESSAGE_H*/