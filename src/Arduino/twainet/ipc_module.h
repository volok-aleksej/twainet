#ifndef IPC_MODULE_H
#define IPC_MODULE_H

#include "ipc_connector.h"
#include "ipc_object_name.h"
#include "ipc_signal_handler.h"

// #include "connector_factory.h"
#include "connector_manager.h"
#include "signal_owner.h"
#include "std/list.hpp"
#include "std/vector.hpp"

#include <string>
#include <stdarg.h>

class IPCModule : protected SignalOwner
{
protected:
	friend class IPCSignalHandler;
	class IPCObject
	{
	public:
		IPCObject();
		IPCObject(const IPCObject& object);
		IPCObject(const IPCObjectName& ipcName, const char* ip = "127.0.0.1", int port = 0, const char* accessId = "");
		~IPCObject();

		bool operator == (const IPCObject& ipcName);
		bool operator != (const IPCObject& ipcName);
		bool operator < (const IPCObject& ipcName) const;
		void operator = (const IPCObject& ipcName);

		IPCObjectName m_ipcName;
		String m_ip;
		int m_port;
		String m_accessId;
	};

public:
// 	IPCModule(const IPCObjectName& moduleName, ConnectorFactory* factory, int ipv);
    IPCModule(const IPCObjectName& moduleName);
	virtual ~IPCModule();

	void DisconnectModule(const IPCObjectName& moduleName);
	bool IsExit();
	void Exit();
	void UpdateModuleName(const IPCObjectName& moduleName);
	void SendMsg(const IPCSignalMessage& msg);
	void CreateInternalConnection(const IPCObjectName& moduleName, const std::string& ip, int port);
	const IPCObjectName& GetModuleName();
 	twnstd::vector<IPCObjectName> GetIPCObjects();
	twnstd::vector<IPCObjectName> GetConnectedModules();
protected:
	virtual twnstd::vector<IPCObjectName> GetTargetPath(const IPCObjectName& target);
	virtual void OnNewConnector(Connector* connector);
	virtual void OnFireConnector(const String& moduleName);
	virtual void OnConnected(const String& moduleName);
	virtual void OnConnectFailed(const String& moduleName);
	virtual void OnMessage(const String& messageName, const twnstd::vector<String>& path, const char* data, unsigned int lenData);
	virtual bool CheckFireConnector(const String& moduleName);
	virtual void FillIPCObjectList(twnstd::list<IPCObject>& ipcList);
	virtual void OnIPCObjectsChanged();
protected:
	void ipcSubscribe(IPCConnector* connector, SignalReceiver* receiver, IReceiverFunc* func);
	void AddConnector(Connector* connector);
protected:
	IPCObjectName m_moduleName;
	twnstd::list<IPCObject> m_ipcObject;	// available modules
	twnstd::list<IPCObject> m_modules;		// connected modules
	ConnectorManager *m_manager;
	bool m_isExit;
	
// 	std::map<std::string, std::vector<std::string> > m_connectors;
private:
	IPCSignalHandler m_ipcSignalHandler;
};
#endif/*IPC_MODULE_H*/