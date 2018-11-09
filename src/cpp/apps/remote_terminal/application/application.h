#ifndef APPLICATION_H
#define APPLICATION_H

#include "include/twainet.h"
#include "include/default_app.h"
#include "module/terminal_module.h"

class ApplicationTerminal : public DefaultApplication<ApplicationTerminal>
{
	friend class Singleton<ApplicationTerminal>;
	ApplicationTerminal();
public:
	~ApplicationTerminal();

	virtual void InitializeApplication();

	static std::string GetAppName();
	static std::string GetDescription();
protected:
    friend class IApplication<ApplicationTerminal>;
	virtual void OnModuleCreationFailed(Twainet::Module module);
	virtual void OnServerCreationFailed(Twainet::Module module);
};

#endif/*APPLICATION_H*/
