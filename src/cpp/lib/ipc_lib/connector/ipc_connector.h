#ifndef IPC_CONNECTOR_H
#define IPC_CONNECTOR_H

#include "connector_lib/connector/connector.h"
#include "connector_lib/connector/connector_manager.h"
#include "connector_lib/signal/signal_receiver.h"
#include "connector_lib/signal/signal_owner.h"
#include "connector_lib/signal/signal_message.h"
#include "connector_lib/message/connector_messages.h"

#include "../thread/ipc_checker_thread.h"
#include "../module/ipc_object_name.h"

#include "ipc_handler.h"
#include "proto_message.h"

class ListenThread;

class IPCConnector : public Connector, public SignalReceiver, protected SignalOwner
{
public:
	IPCConnector(AnySocket* socket, const IPCObjectName& moduleName);
	virtual ~IPCConnector();

	virtual void onNewConnector(const Connector* connector);
	virtual void SubscribeConnector(const IPCConnector* connector);
	virtual void SubscribeModule(::SignalOwner* owner);

	IPCObjectName GetModuleName() const;
	std::string GetAccessId();
protected:
	friend class Signal;
	void onIPCMessage(const IPCProtoMessage& msg);
	void onIPCMessage(const IPCMessageSignal& msg);
	void onModuleNameMessage(const ModuleNameMessage& msg);
	void onModuleStateMessage(const ModuleStateMessage& msg);
	void onUpdateIPCObjectMessage(const UpdateIPCObjectMessage& msg);
	void onChangeIPCNameMessage(const ChangeIPCNameMessage& msg);
	void onRemoveIPCObjectMessage(const RemoveIPCObjectMessage& msg);
	
	void onDisconnected(const DisconnectedMessage& msg);
	void onCreatedListener(const CreatedListenerMessage& msg);
	void onErrorListener(const ListenErrorMessage& msg);
	void onErrorConnect(const ConnectErrorMessage& msg);
	void onAddConnector(const ConnectorMessage& msg);
	void onInitInternalConnectionMessage(const InitInternalConnectionMessage& msg);
	void onInternalConnectionDataSignal(const InternalConnectionDataSignal& msg);

	friend class IPCModule;
	friend class PingThread;
	friend class IPCHandler;
	void onIPCSignal(const DataMessage& msg);
	void addIPCSubscriber(SignalReceiver* receiver, IReceiverFunc* func);
	void ipcSubscribe(IPCConnector* connector, IReceiverFunc* func);
protected:
	virtual void ThreadFunc();
	virtual void OnStart();
	virtual void OnStop();
	virtual bool SendData(char* data, int len);

	virtual bool SetModuleName(const IPCObjectName& moduleName);
	virtual void SetAccessId(const std::string& accessId);
	virtual void OnConnected();
	virtual void OnDisconnected();
	virtual void OnAddIPCObject(const std::string& moduleName);
	virtual void OnUpdateIPCObject(const std::string& oldModuleName, const std::string& newModuleName);
	virtual IPCObjectName GetIPCName();
protected:
	IPCHandler m_handler;
	IPCCheckerThread *m_checker;
	bool m_bConnected;
	bool m_isNotifyRemove;
	bool m_isSendIPCObjects;
	bool m_isCoordinator;
private:
	IPCObjectName m_moduleName;
	std::string m_accessId;
	Signal* m_ipcSignal;
	bool m_isExist;
	std::string m_rand;
	
	//for internal connections
	ConnectorManager *m_manager;
	CriticalSection m_cs;
	std::map<std::string, ListenThread*> m_internalListener;
	ObjectManager<std::string> m_internalConnections;
};

#endif/*IPC_CONNECTOR_H*/