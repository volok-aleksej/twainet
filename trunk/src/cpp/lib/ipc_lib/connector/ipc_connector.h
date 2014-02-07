#ifndef IPC_CONNECTOR_H
#define IPC_CONNECTOR_H

#include "connector_lib/connector/connector.h"
#include "connector_lib/signal/signal_receiver.h"
#include "connector_lib/signal/signal_owner.h"
#include "connector_lib/signal/signal_message.h"
#include "connector_lib/message/connector_messages.h"

#include "../thread/ipc_checker_thread.h"
#include "../module/ipc_object_name.h"

#include "proto_message.h"

#pragma warning(disable:4244 4267)
#include "../messages/ipc.pb.h"
using namespace ipc;
#pragma warning(default:4244 4267)

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

class IPCConnector : public Connector, public SignalReceiver, protected SignalOwner
{
public:
	IPCConnector(AnySocket* socket, const IPCObjectName& moduleName);
	virtual ~IPCConnector();

	virtual void onNewConnector(const Connector* connector);
	virtual void Subscribe(::SignalOwner* owner);

	IPCObjectName GetModuleName() const;
protected:
	friend class Signal;
	void onIPCMessage(const IPCProtoMessage& msg);
	void onIPCMessage(const IPCMessageSignal& msg);
	void onModuleNameMessage(const ModuleNameMessage& msg);
	void onModuleStateMessage(const ModuleStateMessage& msg);
	void onUpdateIPCObjectMessage(const UpdateIPCObjectMessage& msg);
	void onChangeIPCNameMessage(const ChangeIPCNameMessage& msg);
	void onRemoveIPCObjectMessage(const RemoveIPCObjectMessage& msg);

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

	friend class IPCModule;
	friend class PingThread;
	void onIPCSignal(const DataMessage& msg);
	void addIPCSubscriber(SignalReceiver* receiver, IReceiverFunc* func);
	void ipcSubscribe(IPCConnector* connector, IReceiverFunc* func);
protected:
	virtual void ThreadFunc();
	virtual void OnStart();
	virtual void OnStop();
	virtual bool SendData(char* data, int len);

	virtual void OnConnected();
	virtual void OnAddIPCObject(const std::string& moduleName);
	virtual void OnUpdateIPCObject(const std::string& oldModuleName, const std::string& newModuleName);
protected:
	IPCCheckerThread m_checker;
private:
	IPCObjectName m_moduleName;
	Signal* m_ipcSignal;
	bool m_isCoordinator;
	bool m_isExist;
	std::string m_rand;
};

#endif/*IPC_CONNECTOR_H*/