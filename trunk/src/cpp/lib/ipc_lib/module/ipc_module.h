#ifndef IPC_MODULE_H
#define IPC_MODULE_H

#include "ipc_lib/connector/ipc_connector.h"
#include "ipc_lib/module/ipc_object_name.h"

#include "connector_lib/connector/connector_factory.h"
#include "connector_lib/connector/connector_manager.h"
#include "connector_lib/signal/signal_receiver.h"
#include "connector_lib/signal/signal_owner.h"
#include "connector_lib/message/connector_messages.h"
#include "connector_lib/thread/listen_thread.h"

#include "thread_lib/common/object_manager.h"

#include <string>

class IPCListenThread : public ListenThread
{
public:
	IPCListenThread(const ListenAddress& address)
		: ListenThread(address){}

protected:
	void OnStart(){}
	void OnStop(){}
};

class IPCModule : public SignalReceiver, protected SignalOwner
{
public:
	static const std::string m_coordinatorIPCName;
	static const int m_maxTryConnectCount;
protected:
	class IPCObject
	{
	public:
		IPCObject();
		IPCObject(const IPCObjectName& ipcName, const std::string& ip = "127.0.0.1", int port = 0);
		~IPCObject();

		bool operator == (const IPCObject& ipcName);
		void operator = (const IPCObject& ipcName);

		IPCObjectName m_ipcName;
		std::string m_ip;
		int m_port;
	};

	class TryConnectCounter
	{
	public:
		TryConnectCounter();
		TryConnectCounter(const std::string& moduleName);

		bool operator == (const TryConnectCounter& ipcName);
		void operator = (const TryConnectCounter& ipcName);

		int m_count;
		std::string m_moduleName;
	};
public:
	IPCModule(const IPCObjectName& moduleName, ConnectorFactory* factory);
	virtual ~IPCModule();

	void Start();
	void StartAsCoordinator();
	void ConnectTo(const IPCObjectName& moduleName);
	void DisconnectModule(const IPCObjectName& moduleName);
	bool IsExit();
	void Exit();
	void UpdateModuleName(const IPCObjectName& moduleName);
	void SendMsg(const IPCMessageSignal& msg);
	const IPCObjectName& GetModuleName();
	std::vector<IPCObjectName> GetIPCObjects();

	template<class Msg> void SendMsg(const Msg& msg, int countPath, ...);
protected:
	friend class Signal;
	void getListenPort(const ListenerParamMessage& msg);
	void onCreatedListener(const CreatedListenerMessage& msg);
	void onErrorListener(const ListenErrorMessage& msg);
	void onAddConnector(const ConnectorMessage& msg);
	void onErrorConnect(const ConnectErrorMessage& msg);
	void onModuleName(const ModuleNameMessage& msg);
	void onAddIPCObject(const AddIPCObjectMessage& msg);
	void onRemoveIPCObject(const RemoveIPCObjectMessage& msg);
	void onDisconnected(const DisconnectedMessage& msg);
	void onIPCObjectList(const IPCObjectListMessage& msg);
	void onUpdateIPCObject(const UpdateIPCObjectMessage& msg);
	void onIPCMessage(const IPCProtoMessage& msg);
	void onConnected(const ConnectedMessage& msg);

	virtual void OnNewConnector(Connector* connector);
	virtual void OnFireConnector(const std::string& moduleName);
	virtual void OnConnected(const std::string& moduleName);
	virtual void OnConnectFailed(const std::string& moduleName);
	virtual void OnMessage(const std::string& messageName, const std::vector<std::string>& path, const std::string& data);
	virtual bool CheckFireConnector(const std::string& moduleName);
	virtual void ModuleCreationFialed();

protected:
	void Start(const std::string& ip, int port);
	void ConnectToCoordinator();
	void ipcSubscribe(IPCConnector* connector, IReceiverFunc* func);
protected:
	IPCListenThread* m_listenThread;
	ConnectorFactory* m_factory;
	IPCObjectName m_moduleName;
	ObjectManager<IPCObject> m_ipcObject;
	ObjectManager<IPCObject> m_modules;
	ConnectorManager m_manager;
	bool m_isCoordinator;
	bool m_isExit;
	int m_countListener;
	int m_count—onnect;
	ObjectManager<TryConnectCounter> m_tryConnectCounters;
	
	CriticalSection m_csConnectors;
	std::map<std::string, std::vector<std::string>> m_connectors;
};

template<class Msg>
void IPCModule::SendMsg(const Msg& msg, int countPath, ...)
{
	ProtoSendMessage<Msg> protoMsg(msg);

	va_list argptr;
	va_start(argptr, countPath);
	for( ; countPath; countPath--)
	{
		IPCObjectName* ipcPath = (IPCObjectName*)va_arg(argptr, void*);
		*protoMsg.add_ipc_path() = *ipcPath;
	}
	va_end(argptr);

	protoMsg.createMessage();
	IPCMessageSignal ipcMsg(static_cast<IPCMessage&>(protoMsg));
	SendMsg(ipcMsg);
}

#endif/*IPC_MODULE_H*/