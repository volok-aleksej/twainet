#ifndef CLIENT_CONNECTOR_H
#define CLIENT_CONNECTOR_H

#include "ipc_lib/connector/ipc_connector.h"

#pragma warning(disable:4244 4267)
#include "../messages/client_server.pb.h"
#include "../messages/ipc.pb.h"
using namespace client_server;
#pragma warning(default:4244 4267)

class ClientServerConnector;

typedef ProtoMessage<Login, ClientServerConnector> LoginMessage;
typedef ProtoMessage<LoginResult, ClientServerConnector> LoginResultMessage;
typedef ProtoMessage<InitTunnel, ClientServerConnector> InitTunnelMessage;
typedef ProtoMessage<TryConnectTo, ClientServerConnector> TryConnectToMessage;
typedef ProtoMessage<InitTunnelStarted, ClientServerConnector> InitTunnelStartedMessage;
typedef ProtoMessage<InitTunnelComplete, ClientServerConnector> InitTunnelCompleteMessage;
typedef ProtoMessage<PeerData, ClientServerConnector> PeerDataMessage;
typedef SignalMessage<InitTunnel> InitTunnelSignal;
typedef SignalMessage<TryConnectTo> TryConnectToSignal;
typedef SignalMessage<InitTunnelStarted> InitTunnelStartedSignal;
typedef SignalMessage<InitTunnelComplete> InitTunnelCompleteSignal;
typedef SignalMessage<PeerData> PeerDataSignal;

class ClientServerConnector : public IPCConnector
{
public:
	ClientServerConnector(AnySocket* socket, const IPCObjectName& moduleName);
	virtual ~ClientServerConnector();

	virtual void SubscribeConnector(const IPCConnector* connector);
	virtual void SubscribeModule(::SignalOwner* owner);
	virtual void OnConnected();

	void SetUserName(const std::string& userName);
	void SetPassword(const std::string& password);
protected:
	virtual void OnStart();
	virtual bool SetModuleName(const IPCObjectName& moduleName);
	virtual IPCObjectName GetIPCName();
protected:
	friend class Signal;
	void onIPCMessage(const IPCProtoMessage& msg);
	void onInitTunnelSignal(const InitTunnelSignal& msg);
	void onInitTunnelStartedSignal(const InitTunnelStartedSignal& msg);
	void onInitTunnelCompleteSignal(const InitTunnelCompleteSignal& msg);
	void onTryConnectToMessage(const TryConnectToMessage& msg);
	void onTryConnectToSignal(const TryConnectToSignal& msg);
	void onPeerDataSignal(const PeerDataSignal& msg);
	void onIPCObjectListMessage(const IPCObjectListMessage& msg);
	void onAddIPCObjectMessage(const AddIPCObjectMessage& msg);
	void onRemoveIPCObjectMessage(const RemoveIPCObjectMessage& msg);

protected:
	template<typename TMessage, typename THandler> friend class ProtoMessage;
	//Client Messages
	void onMessage(const LoginResult& msg);
	void onMessage(const InitTunnelStarted& msg);

	//Server Messages
	void onMessage(const Login& msg);
	void onMessage(const PeerData& msg);
	void onMessage(const InitTunnelComplete& msg);

	//Client and Server Messages
	void onMessage(const InitTunnel& msg);
	void onMessage(const TryConnectTo& msg);
	void onMessage(const ModuleName& msg);
private:
	std::string m_userName;
	std::string m_password;
	std::string m_ownSessionId;
};

#endif/*CLIENT_CONNECTOR_H*/