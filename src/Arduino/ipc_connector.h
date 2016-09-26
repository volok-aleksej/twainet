#ifndef IPC_CONNECTOR_H
#define IPC_CONNECTOR_H

#include <WString.h>
// #include "connector_lib/message/connector_messages.h"
#include "connector.h"
#include "ipc_checker_thread.h"
#include "ipc_object_name.h"
#include "ipc_handler.h"

class ListenThread;

class IPCConnector : public Connector
{
public:
	IPCConnector(AnySocket* socket, const IPCObjectName& moduleName);
	virtual ~IPCConnector();

	IPCObjectName GetModuleName() const;
	String GetAccessId();
protected:
// 	friend class Signal;
// 	void onIPCMessage(const IPCProtoMessage& msg);
// 	void onIPCMessage(const IPCMessageSignal& msg);
// 	void onModuleNameMessage(const ModuleNameMessage& msg);
// 	void onModuleStateMessage(const ModuleStateMessage& msg);
// 	void onUpdateIPCObjectMessage(const UpdateIPCObjectMessage& msg);
// 	void onChangeIPCNameMessage(const ChangeIPCNameMessage& msg);
// 	void onRemoveIPCObjectMessage(const RemoveIPCObjectMessage& msg);

// 	
// 	friend class IPCModule;
// 	friend class PingThread;
 	friend class IPCHandler;
// 	void onIPCSignal(const DataMessage& msg);
// 	void addIPCSubscriber(SignalReceiver* receiver, IReceiverFunc* func);
// 	void ipcSubscribe(IPCConnector* connector, IReceiverFunc* func);
protected:
 	virtual void ThreadFunc();
 	virtual void OnStart();
 	virtual void OnStop();
	virtual bool SendData(char* data, int len);

	virtual bool SetModuleName(const IPCObjectName& moduleName);
	virtual void SetAccessId(const String& accessId);
	virtual void OnConnected();
	virtual void OnDisconnected();
	virtual void OnAddIPCObject(const String& moduleName);
	virtual void OnUpdateIPCObject(const String& oldModuleName, const String& newModuleName);
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
	String m_accessId;
//	Signal* m_ipcSignal;
	bool m_isExist;
	String m_rand;
};

#endif/*IPC_CONNECTOR_H*/