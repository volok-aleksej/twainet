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
	virtual void OnConfigChanged(const SetConfig& msg);
	
private:
	void ReadConfig();
	
private:
	Twainet::UserPassword m_userPassword;
	std::vector<std::string> m_trustedModules;
};

#endif/*DEAMON_MODULE_H*/