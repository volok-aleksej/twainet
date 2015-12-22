#ifndef DEAMON_MODULE_H
#define DEAMON_MODULE_H

#include "module.h"
#include "client_module_name.h"
#include "thread_lib/common/object_manager.h"

#pragma warning(disable:4244 4267)
#include "../messages/deamon.pb.h"
using namespace deamon;
#pragma warning(default:4244 4267)

#define COORDINATOR_NAME	"twndeamon"

class DeamonModule : public Module
{
public:
	DeamonModule(const Twainet::Module& module);
	virtual ~DeamonModule();
	
protected:
	virtual void OnModuleConnected(const Twainet::ModuleName& moduleName);
	virtual void OnMessageRecv(const Twainet::Message& message);
	virtual void OnModuleListChanged();
protected:
	template<class TMessage, class THandler> friend class DeamonMessage;
	//server messages
	void onMessage(const LocalServerAttributes& msg, const Twainet::ModuleName& path){}
	void onMessage(const ClientNameList& msg, const Twainet::ModuleName& path){}
	
	//client messages
	void onMessage(const ClientName& msg, const Twainet::ModuleName& path);
private:
	void ReadConfig();
private:
	Twainet::UserPassword m_userPassword;
	std::vector<std::string> m_trustedModules;
	ObjectManager<ClientModuleName> m_clientsName;
};

#endif/*DEAMON_MODULE_H*/