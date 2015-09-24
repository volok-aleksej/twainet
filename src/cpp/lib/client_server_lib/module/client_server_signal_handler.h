#ifndef CLIENT_SERVER_SIGNAL_HANDLER_H
#define CLIENT_SERVER_SIGNAL_HANDLER_H

#include "connector_lib/message/connector_messages.h"
#include "client_server_lib/message/client_server_messages.h"
#include "client_server_lib/connector/client_server_connector.h"

class ClientServerModule;

class ClientServerSignalHandler : public SignalReceiver
{
public:
	ClientServerSignalHandler(ClientServerModule* module);
	~ClientServerSignalHandler();
	
	void onAddClientServerConnector(const ConnectorMessage& msg);
	void onErrorConnect(const ConnectErrorMessage& msg);
	void onCreatedListener(const CreatedListenerMessage& msg);
	void onErrorListener(const ListenErrorMessage& msg);
	void onConnected(const ClientServerConnectedMessage& msg);

	void onLogin(const LoginMessage& msg);
	void onLoginResult(const LoginResultMessage& msg);
private:
	ClientServerModule* m_module;
};

#endif/*CLIENT_SERVER_SIGNAL_HANDLER_H*/