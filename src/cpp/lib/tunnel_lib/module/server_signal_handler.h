#ifndef SERVER_SIGNAL_HANDLER_H
#define SERVER_SIGNAL_HANDLER_H

#include "../message/tunnel_messages.h"
#include "client_server_lib/connector/client_server_connector.h"

class TunnelModule;

class ServerSignalHandler : public SignalReceiver
{
public:
	ServerSignalHandler(TunnelModule* module);
	~ServerSignalHandler();
	
	void CheckServers();
	
	void onInitTunnel(const InitTunnelSignal& msg);
	void onPeerData(const PeerDataSignal& msg);
    void onAvailablePearTypes(const AvailablePearTypesSignal& msg);
	void onInitTunnelComplete(const InitTunnelCompleteMessage& msg);

	void onCreatedExternalListener(const CreatedServerListenerMessage& msg);
	void onGotExternalAddress(const GotExternalAddressMessage& msg);
	void onErrorExternalListener(const ListenErrorMessage& msg);
	void onCreatedRelayTCPListener(const CreatedServerListenerMessage& msg);
	void onErrorRelayListener(const ListenErrorMessage& msg);
	void onAddRelayServerConnector(const ConnectorMessage& msg);
	void onCreatedRelayUDPListener(const CreatedServerListenerMessage& msg);
	void onAddRelayUDPServerConnector(const ConnectorMessage& msg);
private:
	TunnelModule* m_module;
};

#endif/*SERVER_SIGNAL_HANDLER_H*/