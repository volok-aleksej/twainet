#ifndef IPC_HANDLER_H
#define IPC_HANDLER_H

#include "include/signal_message.h"
#include "include/proto_message.h"
#include "ipc.pb-c.h"

typedef ProtoMessage<_Ipc__IPCName> IPCNameMessage;
typedef ProtoMessage<_Ipc__IPCMessage> IPCProtoMessage;
typedef ProtoMessage<_Ipc__ModuleName> ModuleNameMessage;
typedef ProtoMessage<_Ipc__AddIPCObject> AddIPCObjectMessage;
typedef ProtoMessage<_Ipc__UpdateIPCObject> UpdateIPCObjectMessage;
typedef ProtoMessage<_Ipc__ChangeIPCName> ChangeIPCNameMessage;
typedef ProtoMessage<_Ipc__RemoveIPCObject> RemoveIPCObjectMessage;
typedef ProtoMessage<_Ipc__IPCObjectList> IPCObjectListMessage;
typedef ProtoMessage<_Ipc__ModuleState> ModuleStateMessage;
typedef ProtoMessage<_Ipc__Ping> PingMessage;

typedef SignalMessage<_Ipc__IPCMessage> IPCSignalMessage;

class IPCConnector;

class IPCHandler
{
public:
	IPCHandler(IPCConnector* connector);
	virtual ~IPCHandler();
	
	void onMessage(const _Ipc__ModuleName& msg);
	void onMessage(const _Ipc__ModuleState& msg);
	void onMessage(const _Ipc__AddIPCObject& msg);
	void onMessage(const _Ipc__RemoveIPCObject& msg);
	void onMessage(const _Ipc__IPCMessage& msg);
 	void onMessage(const _Ipc__IPCObjectList& msg);
	void onMessage(const _Ipc__ChangeIPCName& msg);
	void onMessage(const _Ipc__UpdateIPCObject& msg);
	void onMessage(const _Ipc__Ping& msg);
    void onMessage(const _Ipc__IPCName& msg);
	
private:
	IPCConnector* m_connector;
};

#endif/*IPC_HANDLER_H*/