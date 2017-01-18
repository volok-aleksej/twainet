#ifndef PROTO_MESSAGE_H
#define PROTO_MESSAGE_H

#include "data_message.h"
#include "protobuf-c.h"
#include <malloc.h>

class IPCHandler;

template<typename TMessage, typename THandler = IPCHandler>
class ProtoMessage : public DataMessage
{
    static const char* messageName;
public:
	ProtoMessage(THandler* handler, const ProtobufCMessageDescriptor& descriptor)
		: m_handler(handler), m_descriptor(descriptor), unpacked(false)
    {
        message = (TMessage*)malloc(descriptor.sizeof_message);
        descriptor.message_init((ProtobufCMessage*)message);
    }

	~ProtoMessage()
    {
        if(unpacked)
        {
            protobuf_c_message_free_unpacked((ProtobufCMessage*)message, 0);
        }
        else
        {
            free(message);
        }
    }

	virtual void onMessage()
	{
		m_handler->onMessage(*message);
	}

	virtual bool serialize(char* data, int len)
	{
        TMessage* msg = (TMessage*)protobuf_c_message_unpack(&m_descriptor, 0, len, (const uint8_t*)data);
        if(unpacked)
        {
            protobuf_c_message_free_unpacked((ProtobufCMessage*)message, 0);
        }
        else
        {
            free(message);
        }
        unpacked = true;
        message = msg;
		return true;
	}

	virtual bool deserialize(char* data, int& len)
	{
		int size = protobuf_c_message_get_packed_size((ProtobufCMessage*)message);
		if (size > len)
		{
			len = size;
			return false;
		}

		len = size;
		protobuf_c_message_pack((ProtobufCMessage*)message, (uint8_t*)data);
        return true;
	}

	virtual char* GetName() const
	{
		return (char*)m_descriptor.name;
	}
	
	TMessage* GetMessage()
    {
        return message;
    }
    
    static const char* GetMessageName()
    {
        return messageName;
    }
    
private:
	THandler* m_handler;
	TMessage* message;
    bool unpacked;
    const ProtobufCMessageDescriptor& m_descriptor;
};

#endif/*PROTO_MESSAGE_H*/