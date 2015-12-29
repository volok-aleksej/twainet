#ifndef DEAMON_MODULE_H
#define DEAMON_MODULE_H

#include "module.h"

#define COORDINATOR_NAME	"twndeamon"

class DeamonModule : public Module
{
public:
	DeamonModule();
	virtual ~DeamonModule();
	
protected:
	virtual void OnModuleConnected(const Twainet::ModuleName& moduleName);
protected:
	template<class TMessage, class THandler> friend class DeamonMessage;
	void onMessage(const GetConfig& msg, const Twainet::ModuleName& path);
	void onMessage(const SetConfig& msg, const Twainet::ModuleName& path);
private:
	void ReadConfig();
private:
	Twainet::UserPassword m_userPassword;
	std::vector<std::string> m_trustedModules;
};

#endif/*DEAMON_MODULE_H*/