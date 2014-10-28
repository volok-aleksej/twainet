#ifndef TUNNEL_SERVER_CONNECTOR_H
#define TUNNEL_SERVER_CONNECTOR_H

#include "tunnel_connector.h"
#include "ipc_lib\connector\ipc_connector.h"

class TunnelServerConnector : public IPCConnector
{
public:
	TunnelServerConnector(TunnelConnector* connectorOne, TunnelConnector* connectorTwo);
	virtual ~TunnelServerConnector();

	virtual void Stop();
protected:
	virtual void OnStart();
	virtual void OnStop();
	virtual void ThreadFunc();
protected:
	friend class Signal;
	void onIPCMessageOne(const IPCMessageSignal& msg);
	void onIPCMessageTwo(const IPCMessageSignal& msg);
	void onModuleState(const ModuleStateMessage& msg);
	void onModuleName(const ModuleNameMessage& msg);
private:
	TunnelConnector* m_connectorOne;
	TunnelConnector* m_connectorTwo;
};

#endif/*TUNNEL__SERVER_CONNECTOR_H*/