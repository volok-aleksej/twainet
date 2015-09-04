#ifndef IPC_HANDLER_H
#define IPC_HANDLER_H

#pragma warning(disable:4244 4267)
#include "../messages/ipc.pb.h"
#include "../messages/interconn.pb.h"
using namespace ipc;
using namespace interconn;
#pragma warning(default:4244 4267)

#include "connector_lib/signal/signal_message.h"
#include "proto_message.h"

typedef ProtoMessage<IPCMessage> IPCProtoMessage;
typedef ProtoMessage<ModuleName> ModuleNameMessage;
typedef ProtoMessage<AddIPCObject> AddIPCObjectMessage;
typedef ProtoMessage<UpdateIPCObject> UpdateIPCObjectMessage;
typedef ProtoMessage<ChangeIPCName> ChangeIPCNameMessage;
typedef ProtoMessage<RemoveIPCObject> RemoveIPCObjectMessage;
typedef ProtoMessage<IPCObjectList> IPCObjectListMessage;
typedef ProtoMessage<ModuleState> ModuleStateMessage;
typedef ProtoMessage<Ping> PingMessage;
typedef SignalMessage<IPCMessage> IPCMessageSignal;

typedef ProtoMessage<InitInternalConnection> InitInternalConnectionMessage;
typedef ProtoMessage<InternalConnectionStatus> InternalConnectionStatusMessage;
typedef ProtoMessage<InternalConnectionData> InternalConnectionDataMessage;
typedef SignalMessage<InternalConnectionData> InternalConnectionDataSignal;

class IPCConnector;

class IPCHandler
{
public:
	IPCHandler(IPCConnector* connector);
	virtual ~IPCHandler();
	
	template<typename TMessage, typename THandler> friend class ProtoMessage;
	void onMessage(const ModuleName& msg);
	void onMessage(const ModuleState& msg);
	void onMessage(const AddIPCObject& msg);
	void onMessage(const RemoveIPCObject& msg);
	void onMessage(const IPCMessage& msg);
 	void onMessage(const IPCObjectList& msg);
	void onMessage(const ChangeIPCName& msg);
	void onMessage(const UpdateIPCObject& msg);
	void onMessage(const Ping& msg);
	
	void onMessage(const InitInternalConnection& msg);
	void onMessage(const InternalConnectionStatus& msg);
	void onMessage(const InternalConnectionData& msg);
private:
	IPCConnector* m_connector;
};

#endif/*IPC_HANDLER_H*/