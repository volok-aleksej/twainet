#include "client_server_messages.h"

ClientServerConnectedMessage::ClientServerConnectedMessage(const std::string& id)
: ConnectedMessage(id)
{
}

std::string ClientServerConnectedMessage::GetMessageName()
{
	return typeid(ClientServerConnectedMessage).name();
}

