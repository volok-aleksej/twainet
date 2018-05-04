#ifndef CONNECTOR_MANAGER_H
#define CONNECTOR_MANAGER_H

#include "connector.h"
#include "include/object_manager.h"
#include "thread_lib/common/managers_container.h"
#include "connector_lib/signal/signal.h"
#include "connector_lib/signal/signal_owner.h"

class ConnectorManager : public SignalOwner, public DynamicManager
{
public:
	ConnectorManager();
	~ConnectorManager();

	void AddConnection(Connector* conn);
	void StopConnection(const std::string& moduleName);
	void StopAllConnection();

protected:
    class ModuleInfo
    {
    public:
        ModuleInfo(const std::string& id, const std::string& connid, const std::string& accessid)
        : m_id(id), m_connid(connid), m_accessid(accessid){}
        std::string m_id;
        std::string m_connid;
        std::string m_accessid;
    };

	template<typename TClass, typename TFunc, typename TReturn> friend class Reference;
	template<typename TClass, typename TFunc, typename TObject, typename TReturn> friend class ReferenceObject;
	bool CheckConnection(const std::vector<ModuleInfo>& discModules, const Connector* msg);
	bool StopConnector(const std::vector<ModuleInfo>& discModules, const Connector* msg);
	bool StopConnectors(const Connector* connector);
	bool SubscribeConnector(const Connector* newConnector, const Connector* connector);
	bool StopConnectionByName(const std::string& moduleName, const Connector* connector);
protected:
	virtual void ManagerFunc();
	virtual void ManagerStart();
	virtual void ManagerStop();
private:
	ObjectManager<Connector*> m_connectors;
};

#endif/*CONNECTOR_MANAGER_H*/
