#ifndef TUNNEL_MODULE_H
#define TUNNEL_MODULE_H

#include "tunnel_containers.h"
#include "../message/tunnel_messages.h"
#include "../thread/tunnel_checker_thread.h"
#include "client_server_lib/module/client_server_module.h"

class TunnelModule : public ClientServerModule
{
public:
	static const std::string m_tunnelIPCName;
	static const std::string m_tunnelAccessId;
public:
	TunnelModule(const IPCObjectName& ipcName, ConnectorFactory* factory);
	virtual ~TunnelModule();
public:
	void InitNewTunnel(const std::string& extSessionId, TunnelConnector::TypeConnection type);
	void DestroyTunnel(const std::string& extSessionId);
	void SetTypeTunnel(const std::string& oneSessionId, const std::string& twoSessionId, TunnelConnector::TypeConnection type);
protected:
	virtual void OnNewConnector(Connector* connector);
	virtual void OnTunnelConnectFailed(const std::string& sessionId);
	virtual void OnTunnelConnected(const std::string& sessionId, TunnelConnector::TypeConnection type);
	virtual void FillIPCObjectList(IPCObjectListMessage& msg);
protected:
	//for client
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

	//for server
	void onInitTunnel(const InitTunnelSignal& msg);
	void onPeerData(const PeerDataSignal& msg);
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
	void CreateLocalListenThread(const std::string& extSessionId);
	void CreateLocalUDPSocket(const std::string& extSessionId);
	void CreateLocalConnectThread(const std::string& extSessionId, const std::string& ip, int port, bool isTCP);
	void InitExternalConnectThread(const std::string& extSessionId, const std::string& ip, int port);
	void CreateExternalConnectThread(const std::string& extSessionId, const std::string& ip, int port);
	void CreateRelayConnectThread(const std::string& extSessionId, const std::string& ip, int port, bool isTCP);

	friend class TunnelCheckerThread;
	void CheckTunnels();
private:
	std::map<std::string, TunnelConnect*> m_tunnels;
	std::map<std::string, TunnelServer*> m_servers;
	CriticalSection m_cs;
	ObjectManager<PeerType> m_typePeers;
	ObjectManager<TunnelStep> m_tunnelsStep;
	TunnelCheckerThread* m_tunnelChecker;
};

#endif/*TUNNEL_MODULE_H*/