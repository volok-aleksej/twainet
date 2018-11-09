#ifndef DEAMON_MODULE_H
#define DEAMON_MODULE_H

#include "include/module.h"

#define COORDINATOR_NAME	"twndeamon"

class DeamonModule;

typedef UserMessage<Test, DeamonModule> TestMessage;

class DeamonModule : public Module
{
public:
	DeamonModule();
	virtual ~DeamonModule();
    
    void Init();
	
protected:
	virtual void OnModuleConnected(const Twainet::ModuleName& moduleName);
	
protected:
	virtual void OnConfigChanged(const SetConfig& msg);
	
private:
	template<class TMessage, class THandler> friend class UserMessage;
	void onMessage(const Test& test, Twainet::ModuleName path);
private:
	void ReadConfig();
	
private:
	Twainet::UserPassword m_userPassword;
	std::vector<std::string> m_trustedModules;
    std::vector<std::string> m_pluginPaths;
};

#endif/*DEAMON_MODULE_H*/
