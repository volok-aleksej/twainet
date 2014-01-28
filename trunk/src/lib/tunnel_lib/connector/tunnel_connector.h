#ifndef TUNNEL_CONNECTOR_H
#define TUNNEL_CONNECTOR_H

#include "ipc_lib\connector\ipc_connector.h"
#include "ipc_lib\thread\ping_thread.h"

#pragma warning(disable:4244 4267)
#include "messages/client_server.pb.h"
using namespace client_server;
#pragma warning(default:4244 4267)

class TunnelConnector;

typedef HandlerProtoMessage<TunnelConnector, Test> TestMessage;

class TunnelConnector : public IPCConnector
{
	friend class TunnelServerConnector;
public:
	TunnelConnector(AnySocket* socket, const IPCObjectName& moduleName);
	virtual ~TunnelConnector();

	void SetServerFlag();
protected:
	virtual void OnStart();
	virtual void OnStop();
protected:
	template<typename THandler, typename TMessage> friend class HandlerProtoMessage;
	template<typename TMessage, typename THandler> friend class ProtoMessage;
	void onMessage(const Test& msg);
	void onMessage(const ModuleName& msg);
	void onMessage(const ModuleState& msg);
private:
	bool m_isServer;
	PingThread m_pingThread;
};

#endif/*TUNNEL_CONNECTOR_H*/