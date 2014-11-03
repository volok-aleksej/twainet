#ifndef TUNNEL_MODULE_H
#define TUNNEL_MODULE_H

#include "tunnel_containers.h"
#include "../message/tunnel_messages.h"
#include "../thread/tunnel_checker_thread.h"
#include "client_server_lib/module/client_server_module.h"

class TunnelModule : public ClientServerModule
{
public:
	TunnelModule(const IPCObjectName& ipcName, ConnectorFactory* factory);
	virtual ~TunnelModule();
public:
	void InitNewTunnel(const std::string& extSessionId);
	void DestroyTunnel(const std::string& extSessionId);
protected:
	virtual void OnNewConnector(Connector* connector);
	virtual void TunnelConnectFailed(const std::string& sessionId);
	virtual void TunnelConnected(const std::string& sessionId);
protected:
	//for client
	void onInitTunnel(const InitTunnelMessage& msg);
	void onTryConnectTo(const TryConnectToMessage& msg);
	void onInitTunnelStarted(const InitTunnelStartedMessage& msg);

	void onCreatedLocalListener(const CreatedListenerMessage& msg);
	void onErrorLocalListener(const ListenErrorMessage& msg);
	void onErrorLocalConnect(const ConnectErrorMessage& msg);
	void onAddLocalConnector(const ConnectorMessage& msg);
	void onErrorExternalConnect(const ConnectErrorMessage& msg);
	void onAddExternalConnector(const ConnectorMessage& msg);
	void onErrorRelayConnect(const ConnectErrorMessage& msg);
	void onAddRelayConnector(const ConnectorMessage& msg);

	void onModuleName(const ModuleNameMessage& msg);

	//for server
	void onInitTunnel(const InitTunnelSignal& msg);

	void onCreatedExternalListener(const CreatedServerListenerMessage& msg);
	void onGotExternalAddress(const GotExternalAddressMessage& msg);
	void onErrorExternalListener(const ListenErrorMessage& msg);
	void onCreatedRelayListener(const CreatedServerListenerMessage& msg);
	void onErrorRelayListener(const ListenErrorMessage& msg);
	void onAddRelayServerConnector(const ConnectorMessage& msg);

private:
	void CreateLocalListenThread(const std::string& extSessionId);
	void CreateLocalConnectThread(const std::string& extSessionId, const std::string& ip, int port);
	void InitExternalConnectThread(const std::string& extSessionId, const std::string& ip, int port);
	void CreateExternalConnectThread(const std::string& extSessionId, const std::string& ip, int port);
	void CreateRelayConnectThread(const std::string& extSessionId, const std::string& ip, int port);

	friend class TunnelCheckerThread;
	void CheckTunnels();
private:
	std::map<std::string, TunnelConnect*> m_tunnels;
	std::map<std::string, TunnelServer*> m_servers;
	CriticalSection m_cs;
	TunnelCheckerThread* m_tunnelChecker;
};

#endif/*TUNNEL_MODULE_H*/