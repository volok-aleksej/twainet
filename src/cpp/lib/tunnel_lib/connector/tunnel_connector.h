#ifndef TUNNEL_CONNECTOR_H
#define TUNNEL_CONNECTOR_H

#include "ipc_lib/connector/ipc_connector.h"
#include "ipc_lib/thread/ping_thread.h"

#pragma warning(disable:4244 4267)
#include "../messages/client_server.pb.h"
using namespace client_server;
#pragma warning(default:4244 4267)

class TunnelConnector;

class TunnelConnector : public IPCConnector
{
	friend class TunnelServerConnector;
public:
	enum TypeConnection
	{
		UNKNOWN,
		LOCAL_TCP,
		LOCAL_UDP,
		EXTERNAL,
		RELAY_TCP,
		RELAY_UDP
	};

	TunnelConnector(AnySocket* socket, const IPCObjectName& moduleName);
	virtual ~TunnelConnector();

	void SetServerFlag();
	void SetTypeConnection(TypeConnection type);
	TypeConnection GetTypeConnection();
protected:
	virtual void OnStart();
	virtual void OnStop();
	virtual bool SetModuleName(const IPCObjectName& moduleName);
	virtual void OnConnected();

protected:
	template<typename TMessage, typename THandler> friend class ProtoMessage;
	void onMessage(const ModuleName& msg);
	void onMessage(const ModuleState& msg);
private:
	bool m_isServer;
	PingThread *m_pingThread;
	TypeConnection m_type;
};

#endif/*TUNNEL_CONNECTOR_H*/