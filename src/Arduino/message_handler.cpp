#include "message_handler.h"
#include <malloc.h>
#include <string.h>

MessageHandler::MessageHandler()
{
}

MessageHandler::~MessageHandler(void)
{
}

void MessageHandler::addMessage(DataMessage* msg)
{
	m_messages.push_back(msg);
}

bool MessageHandler::onData(char* data, int len)
{
	int typeLen = 0;
	memcpy(&typeLen, data, sizeof(int));
	int headerLen = sizeof(int) + typeLen;
	if (typeLen
		&& len >= headerLen)
	{
		char* type = (char*)malloc(typeLen + 1);
		memset(type, 0, typeLen + 1);
		memcpy(type, data + sizeof(unsigned int), typeLen);
		bool ret = onData(type, data + headerLen, len - headerLen);
        free(type);
        return ret;
	}
	return false;
}

bool MessageHandler::toMessage(const DataMessage& msg)
{
	char* data = 0;
	int len = 0;
	deserialize(msg, data, len);
	data = (char*)malloc(len);
	bool res = deserialize(msg, data, len) && SendData(data, len);
	free(data);
	return res;
}

bool MessageHandler::deserialize(const DataMessage& msg, char* data, int& len)
{
	char* type = msg.GetDeserializeName();
	int typeLen = (int)strlen(type);
	int headerLen = sizeof(int) + typeLen;
	int msgBodyLen = (len > headerLen ? (len - headerLen) : 0);

	bool res = const_cast<DataMessage&>(msg).deserialize(data + headerLen, msgBodyLen);
	len = headerLen + msgBodyLen;

	if(res && data)
	{
		memcpy(data, &typeLen, sizeof(unsigned int));
		memcpy(data + sizeof(unsigned int), type, typeLen);
	}

	return res;
}

bool MessageHandler::onData(char* type, char* data, int len)
{
    if(len < 0) {
        return false;
    }
    for (unsigned int i = 0; i < m_messages.length(); ++i)
    {
        if(strcmp(type, m_messages[i]->GetName()) == 0)
        {
            m_messages[i]->serialize(data, len);
            m_messages[i]->onMessage();
            return true;
        }
    }
	return false;
}