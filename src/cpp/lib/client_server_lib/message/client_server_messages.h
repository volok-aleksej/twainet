#ifndef CLIENT_SERVER_MESSAGES_H
#define CLIENT_SERVER_MESSAGES_H

#include "connector_lib\message\connector_messages.h"

class ClientServerConnectedMessage : public ConnectedMessage
{
public:
	ClientServerConnectedMessage(const std::string& id);
	static std::string GetMessageName();
};

#endif/*CLIENT_SERVER_MESSAGES_H*/