#ifndef TWAINET_MODULE_H
#define TWAINET_MODULE_H

#include "tunnel_lib/module/tunnel_module.h"

class TwainetModule : public TunnelModule
{
public:
	TwainetModule(const IPCObjectName& ipcName, int ipv);
	~TwainetModule();
protected:
	// Tunnel with other computer was not created
	virtual void OnTunnelConnectFailed(const std::string& sessionId);

	// Tunnel with other computer was created
	virtual void OnTunnelConnected(const std::string& sessionId, TunnelConnector::TypeConnection type);
	
	// Client was connected to server
	virtual void OnServerConnected();

	// Server was connected to client
	virtual void OnClientConnector(const std::string& sessionId);
	
	// connect with module failed
	virtual void OnConnectFailed(const std::string& moduleName);

	// connect with server is successful, but login is failed
	virtual void OnAuthFailed();

	// Connection with module was created.
	virtual void OnConnected(const std::string& moduleName);

	// Connection with module was destroyed.
	// Tunnel connection this is the module's connection also,
	// therefore this function will called also
	// Client or server connection this is the module's connection also,
	// therefore this function will called also
	virtual void OnFireConnector(const std::string& moduleName);

	// It has got message
	virtual void OnMessage(const std::string& messageName, const std::vector<std::string>& path, const std::string& data);

	// Module has not created
	virtual void ModuleCreationFialed();
	
	// Server has not created
	virtual void ServerCreationFialed();

	// Changed internal connection status
	virtual void OnInternalConnection(const std::string& moduleName, const std::string& id, ConnectionStatus status, int port);
};

#endif/*TWAINET_MODULE_H*/