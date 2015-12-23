#ifndef GOOGLE_DATA_MODULE_H
#define GOOGLE_DATA_MODULE_H

#include "module.h"
#include "client_module_name.h"
#include "thread_lib/common/object_manager.h"

#pragma warning(disable:4244 4267)
#include "../messages/deamon.pb.h"
using namespace deamon;
#pragma warning(default:4244 4267)

#define GOOGLE_DATA_NAME	"googledata"

class GoogleDataModule : public Module
{
public:
	GoogleDataModule(const Twainet::Module& module);
	virtual ~GoogleDataModule();
protected:
	template<class TMessage, class THandler> friend class DeamonMessage;
	void onMessage(const LocalServerAttributes& msg, const Twainet::ModuleName& path);
	void onMessage(const ClientNameList& msg, const Twainet::ModuleName& path);
	void onMessage(const ClientName& msg, const Twainet::ModuleName& path){}
protected:
	virtual void OnModuleConnectionFailed(const Twainet::ModuleName& moduleId);
	virtual void OnServerConnected(const char* sessionId);
	virtual void OnMessageRecv(const Twainet::Message& message);
private:
	std::string m_sessionId;
	ObjectManager<ClientModuleName> m_clientsName;
};

#endif/*GOOGLE_DATA_MODULE_H*/