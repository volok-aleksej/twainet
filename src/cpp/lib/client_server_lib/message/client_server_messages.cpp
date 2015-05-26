#include "client_server_messages.h"

ClientServerConnectedMessage::ClientServerConnectedMessage(const std::string& id)
: ConnectedMessage(id, false)
{
}

std::string ClientServerConnectedMessage::GetMessageName()
{
	return typeid(ClientServerConnectedMessage).name();
}

