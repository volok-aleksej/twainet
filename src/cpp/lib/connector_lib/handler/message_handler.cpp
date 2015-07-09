#include "message_handler.h"
#ifndef WIN32
#	include <string.h>
#endif/*WIN32*/

MessageHandler::~MessageHandler(void)
{
	std::map<std::string, DataMessage*>::iterator it;
	for (it = m_messages.begin(); it != m_messages.end(); ++it)
	{
		delete it->second;
	}

	m_messages.clear();
}

void MessageHandler::addMessage(DataMessage* msg)
{
	m_messages[msg->GetName()] = msg;
}

bool MessageHandler::onData(char* data, int len)
{
	int typeLen = 0;
	memcpy(&typeLen, data, sizeof(int));
	int headerLen = sizeof(int) + typeLen;
	if (typeLen
		&& len >= (unsigned int)headerLen)
	{
		std::string type("", typeLen);
		memcpy((char*)type.c_str(), data + sizeof(size_t), typeLen);
		std::map<std::string, DataMessage*>::iterator it = m_messages.find(type);
		if (it != m_messages.end())
		{
			int dataLen = len - headerLen;
			if (dataLen >= 0)
			{
				it->second->serialize(data + headerLen, dataLen);
				it->second->onMessage();
				return true;
			}
		}
	}
	return false;
}

bool MessageHandler::toMessage(const DataMessage& msg)
{
	char* data = 0;
	int len = 0;
	deserialize(msg, data, len);
	data = new char[len];
	bool res = deserialize(msg, data, len) && SendData(data, len);
	delete data;
	return res;
}

bool MessageHandler::deserialize(const DataMessage& msg, char* data, int& len)
{
	std::string type = msg.GetDeserializeName();
	int typeLen = (int)type.size();
	int headerLen = sizeof(int) + typeLen;
	int msgBodyLen = (len > headerLen ? (len - headerLen) : 0);

	bool res = const_cast<DataMessage&>(msg).deserialize(data + headerLen, msgBodyLen);
	len = headerLen + msgBodyLen;

	if(res && data)
	{
		memcpy(data, &typeLen, sizeof(size_t));
		memcpy(data + sizeof(size_t), type.c_str(), typeLen);
	}

	return res;
}
