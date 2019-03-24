#ifndef IPC_MODULE_H
#define IPC_MODULE_H

#include "ipc_lib/connector/ipc_connector.h"
#include "ipc_lib/module/ipc_object_name.h"
#include "ipc_lib/module/ipc_signal_handler.h"

#include "connector_lib/connector/connector_factory.h"
#include "connector_lib/connector/connector_manager.h"
#include "connector_lib/signal/signal_owner.h"
#include "connector_lib/thread/listen_thread.h"

#include "include/object_manager.h"

#include <string>
#include <stdarg.h>

class BaseListenThread : public ListenThread
{
public:
	BaseListenThread(const ListenAddress& address)
		: ListenThread(address){}

protected:
	void OnStart(){}
	void OnStop(){}
};

class IPCModule : protected SignalOwner, public DynamicManager
{
public:
	static const std::string m_coordinatorIPCName;
	static const std::string m_baseAccessId;
	static const std::string m_internalAccessId;
	static const int m_maxTryConnectCount;
	static const int m_connectTimeout;

	class IPCObject
	{
	public:
		IPCObject();
		IPCObject(const IPCObject& object);
		IPCObject(const IPCObjectName& ipcName, const std::string& ip = "127.0.0.1", int port = 0, const std::string& accessId = m_baseAccessId);
		~IPCObject();

		bool operator == (const IPCObject& ipcName);
		bool operator != (const IPCObject& ipcName);
		bool operator < (const IPCObject& ipcName) const;
		void operator = (const IPCObject& ipcName);

		IPCObjectName m_ipcName;
		std::string m_ip;
		int m_port;
		std::string m_accessId;
	};
protected:
	friend class IPCSignalHandler;
	class TryConnectCounter
	{
	public:
		TryConnectCounter();
		TryConnectCounter(const std::string& moduleName);

		bool operator == (const TryConnectCounter& ipcName);
		bool operator != (const TryConnectCounter& ipcName);
		bool operator < (const TryConnectCounter& ipcName) const;
		void operator = (const TryConnectCounter& ipcName);

		int m_count;
		std::string m_moduleName;
	};
public:
	IPCModule(const IPCObjectName& moduleName, ConnectorFactory* factory, int ipv);
	virtual ~IPCModule();

	void Start();
	void StartAsCoordinator();
	void ConnectTo(const IPCObjectName& moduleName);
	void DisconnectModule(const IPCObjectName& moduleName);
	bool IsExit();
	void Exit();
	void UpdateModuleName(const IPCObjectName& moduleName);
	void SendMsg(const IPCMessageSignal& msg);
	void CreateInternalConnection(const IPCObjectName& moduleName, const std::string& ip, int port);
	const IPCObjectName& GetModuleName();
 	std::vector<IPCObjectName> GetIPCObjects();
	std::vector<IPCObjectName> GetConnectedModules();
	std::vector<IPCObjectName> GetInternalConnections();
protected:
	virtual std::vector<IPCObjectName> GetTargetPath(const IPCObjectName& target);
	virtual void OnNewConnector(Connector* connector);
	virtual void OnFireConnector(const std::string& moduleName);
	virtual void OnConnected(const std::string& moduleName);
	virtual void OnConnectFailed(const std::string& moduleName);
	virtual void OnMessage(const std::string& messageName, const std::vector<std::string>& path, const std::string& data);
	virtual bool CheckFireConnector(const std::string& moduleName);
	virtual void ModuleCreationFailed();
	virtual void FillIPCObjectList(std::vector<IPCObject>& ipcList);
	virtual void OnInternalConnection(const std::string& moduleName, ConnectionStatus status, int port);
	virtual void OnIPCObjectsChanged();

	virtual void ManagerFunc();
	virtual void ManagerStart();
	virtual void ManagerStop();
protected:
	void Start(const std::string& ip, int port);
	void ConnectToCoordinator();
	void ipcSubscribe(IPCConnector* connector, SignalReceiver* receiver, IReceiverFunc* func);
	void AddConnector(Connector* connector);
protected:
	BaseListenThread* m_listenThread;
	ConnectorFactory* m_factory;
	IPCObjectName m_moduleName;
	ObjectManager<IPCObject> m_ipcObject;	// available modules
	ObjectManager<IPCObject> m_modules;		// connected modules
	ConnectorManager *m_manager;
	bool m_isCoordinator;
	std::string m_coordinatorName;
	bool m_isExit;
	int m_countListener;
	int m_countConnect;
	int m_ipv;
	ObjectManager<TryConnectCounter> m_tryConnectCounters;

	CriticalSection m_csConnectors;
	std::map<std::string, std::vector<std::string> > m_connectors;
private:
	CriticalSection m_csRequest;
	bool m_bConnectToCoordinatorRequest;
	time_t m_requestCreated;
	IPCSignalHandler m_ipcSignalHandler;
};
#endif/*IPC_MODULE_H*/
