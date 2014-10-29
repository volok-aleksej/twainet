#ifndef TWAINET_MODULE_H
#define TWAINET_MODULE_H

#include "tunnel_lib\module\tunnel_module.h"

class TwainetModule : public TunnelModule
{
public:
	TwainetModule(const IPCObjectName& ipcName);
	~TwainetModule();
protected:
	// Tunnel with other computer was not created
	virtual void TunnelConnectFailed(const std::string& sessionId);

	// Tunnel with other computer was created
	virtual void TunnelConnected(const std::string& sessionId);

	// Client was connected to server
	virtual void OnServerConnected();
	
	// connect with module failed
	virtual void OnConnectFailed(const std::string& moduleName);

	// Connection with module was destroyed.
	// Tunnel connection this is the module's connection also,
	// therefore this function will called also
	// Tunnel connection this is the module's connection also,
	// therefore this function will called also
	virtual void OnFireConnector(const std::string& moduleName);

};

#endif/*TWAINET_MODULE_H*/