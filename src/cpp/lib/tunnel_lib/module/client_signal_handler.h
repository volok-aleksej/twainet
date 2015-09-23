#ifndef CLIENT_SIGNAL_HANDLER_H
#define CLIENT_SIGNAL_HANDLER_H

#include "../message/tunnel_messages.h"
#include "client_server_lib/connector/client_server_connector.h"

class TunnelModule;

class ClientSignalHandler : public SignalReceiver
{
public:
	ClientSignalHandler(TunnelModule* module);
	~ClientSignalHandler();
	
	void onInitTunnel(const InitTunnelMessage& msg);
	void onTryConnectTo(const TryConnectToMessage& msg);
	void onInitTunnelStarted(const InitTunnelStartedMessage& msg);
	
	void onCreatedLocalListener(const CreatedListenerMessage& msg);
	void onErrorLocalListener(const ListenErrorMessage& msg);
	void onErrorLocalTCPConnect(const ConnectErrorMessage& msg);
	void onErrorLocalUDPConnect(const ConnectErrorMessage& msg);
	void onAddLocalTCPConnector(const ConnectorMessage& msg);
	void onAddLocalUDPConnector(const ConnectorMessage& msg);
	void onErrorExternalConnect(const ConnectErrorMessage& msg);
	void onAddExternalConnector(const ConnectorMessage& msg);
	void onAddRelayTCPConnector(const ConnectorMessage& msg);
	void onAddRelayUDPConnector(const ConnectorMessage& msg);
	void onErrorRelayTCPConnect(const ConnectErrorMessage& msg);
	void onErrorRelayUDPConnect(const ConnectErrorMessage& msg);
	
	void onModuleName(const ModuleNameMessage& msg);
	void onConnected(const TunnelConnectedMessage& msg);
private:
	TunnelModule* m_module;
};

#endif/*CLIENT_SIGNAL_HANDLER_H*/