#ifndef IPC_SIGNAL_HANDLER_H
#define IPC_SIGNAL_HANDLER_H

#include "connector_messages.h"
#include "ipc_connector.h"

class IPCModule;

class IPCSignalHandler : public SignalReceiver
{
public:
	IPCSignalHandler(IPCModule* module);
	~IPCSignalHandler();
	void onAddConnector(const ConnectorMessage& msg);
	void onModuleName(const ModuleNameMessage& msg);
	void onAddIPCObject(const AddIPCObjectMessage& msg);
	void onRemoveIPCObject(const RemoveIPCObjectMessage& msg);
	void onDisconnected(const DisconnectedMessage& msg);
	void onIPCObjectList(const IPCObjectListMessage& msg);
	void onUpdateIPCObject(const UpdateIPCObjectMessage& msg);
	void onIPCMessage(const IPCProtoMessage& msg);
    void onIPCMessage(const IPCSignalMessage& msg);
	void onConnected(const ConnectedMessage& msg);
private:
	IPCModule* m_module;
};

#endif/*IPC_SIGNAL_HANDLER_H*/