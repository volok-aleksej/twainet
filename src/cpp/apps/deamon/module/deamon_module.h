#ifndef DEAMON_MODULE_H
#define DEAMON_MODULE_H

#include "module.h"

class DeamonModule : public Module
{
public:
	DeamonModule(const Twainet::Module& module);
	virtual ~DeamonModule();
	
protected:
	virtual void OnModuleConnected(const Twainet::ModuleName& moduleName);
	virtual void OnMessageRecv(const Twainet::Message& message);
	
};

#endif/*DEAMON_MODULE_H*/