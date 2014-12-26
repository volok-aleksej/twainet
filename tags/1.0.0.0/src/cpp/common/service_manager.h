#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#ifdef WIN32

#include <string>
#include <windows.h>

class ServiceManager
{
public:
	ServiceManager(const std::string& serviceName);
	~ServiceManager();

	bool Start();
	bool Stop();
	bool Install(const std::string& descr, const std::string& binPath);
	bool Remove();
protected:
	bool OpenService();
	bool QueryStatus(SERVICE_STATUS_PROCESS& ssp);
private:
	std::string m_serviceName;
	SC_HANDLE m_scManager;
	SC_HANDLE m_service;
};

#endif/*WIN32*/

#endif/*SERVICE_MANAGER_H*/