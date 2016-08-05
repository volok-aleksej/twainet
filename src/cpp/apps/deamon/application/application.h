#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <vector>
#include "include/module.h"
#include "include/default_app.h"

class DeamonApplication : public DefaultApplication<DeamonApplication>
{
	friend class Singleton<DeamonApplication>;
	DeamonApplication();
public:
	virtual ~DeamonApplication();
	
	virtual void InitializeApplication();
	
	static std::string GetAppName();
	static std::string GetDescription();
protected:
    friend class IApplication<DeamonApplication>;
	virtual void OnModuleCreationFailed(Twainet::Module module);
	virtual void OnServerCreationFailed(Twainet::Module module);
};

#endif/*APPLICATION_H*/