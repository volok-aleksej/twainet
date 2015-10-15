#ifndef TUNNEL_MODULE_H
#define TUNNEL_MODULE_H

#include "tunnel_containers.h"
#include "client_signal_handler.h"
#include "server_signal_handler.h"
#include "../thread/tunnel_checker_thread.h"
#include "client_server_lib/module/client_server_module.h"

class TunnelModule : public ClientServerModule
{
public:
	static const std::string m_tunnelIPCName;
	static const std::string m_tunnelAccessId;
public:
	TunnelModule(const IPCObjectName& ipcName, ConnectorFactory* factory, int ipv);
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
private:
	friend class ClientSignalHandler;
	friend class ServerSignalHandler;
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
	ClientSignalHandler m_clientSignalHandler;
	ServerSignalHandler m_serverSignalHandler;
};

#endif/*TUNNEL_MODULE_H*/