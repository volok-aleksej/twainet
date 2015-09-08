#ifndef DEAMON_MODULE_H
#define DEAMON_MODULE_H

#include "module.h"

#pragma warning(disable:4244 4267)
#include "../messages/deamon.pb.h"
using namespace deamon;
#pragma warning(default:4244 4267)

class DeamonModule;

typedef DeamonMessage<LocalServerAttributes, DeamonModule> LocalServerAttributesMessage;

class DeamonModule : public Module
{
public:
	DeamonModule(const Twainet::Module& module);
	virtual ~DeamonModule();
	
protected:
	virtual void OnModuleConnected(const Twainet::ModuleName& moduleName);
	virtual void OnMessageRecv(const Twainet::Message& message);
protected:
	template<class TMessage, class THandler> friend class DeamonMessage;
	void onMessage(const LocalServerAttributes& msg, const std::vector<Twainet::ModuleName>& path);
private:
	Twainet::UserPassword m_userPassword;
};

#endif/*DEAMON_MODULE_H*/