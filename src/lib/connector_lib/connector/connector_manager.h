#ifndef CONNECTOR_MANAGER_H
#define CONNECTOR_MANAGER_H

#include "connector.h"

#include "thread_lib/thread/thread_impl.h"
#include "thread_lib/common/object_manager.h"
#include "../signal/signal.h"
#include "../signal/signal_owner.h"

class ConnectorManager : public SignalOwner, public ThreadImpl
{
public:
	ConnectorManager();
	~ConnectorManager();

	void AddConnection(Connector* conn);
	void StopConnection(const std::string& moduleName);
	void StopAllConnection();
	virtual void Stop();

protected:
	template<typename TClass, typename TFunc> friend class Reference;
	template<typename TClass, typename TFunc, typename TObject> friend class ReferenceObject;
	bool CheckConnection(const std::map<std::string, std::string>& discModules, const Connector* msg);
	bool StopConnector(const std::map<std::string, std::string>& discModules, const Connector* msg);
	void StopConnectors(const Connector* connector);
	void SubscribeConnector(const Connector* newConnector, const Connector* connector);
	void StopConnectionByName(const std::string& moduleName, const Connector* connector);
protected:
	virtual void ThreadFunc();
	virtual void OnStart();
	virtual void OnStop();
private:
	ObjectManager<Connector*> m_connectors;
};

#endif/*CONNECTOR_MANAGER_H*/