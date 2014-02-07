#include "service_manager.h"

#ifdef WIN32

ServiceManager::ServiceManager(const std::string& serviceName)
: m_serviceName(serviceName)
{
	m_scManager = ::OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(::GetLastError())
	{
		m_scManager = NULL;
	}
}

ServiceManager::~ServiceManager()
{
	if (NULL != m_scManager) {
		::CloseServiceHandle(m_scManager);
		m_scManager = NULL;
	}
}

bool ServiceManager::Start()
{
	if(NULL == m_scManager)
	{
		return false;
	}

	OpenService();
	if(m_service)
	{
		SERVICE_STATUS_PROCESS ssp = {0};
		SERVICE_STATUS ss;
		DWORD oldCheckPoint;
		DWORD startTickCount;
		DWORD waitTime;
		if (!QueryStatus(ssp))
		{
			return false;
		}

		if(ssp.dwCurrentState == SERVICE_STOPPED && !::StartServiceA(m_service, 0, NULL))
		{
			return false;
		}
		if(ssp.dwCurrentState == SERVICE_PAUSED && !::ControlService(m_service, SERVICE_CONTROL_CONTINUE, &ss))
		{
			return false;
		}

		startTickCount = GetTickCount();
		oldCheckPoint = ssp.dwCheckPoint;
		while (ssp.dwCurrentState == SERVICE_START_PENDING)
		{
			waitTime = ssp.dwWaitHint / 10;
			if(waitTime < 1000)
				waitTime = 1000;
			else if (waitTime > 10000)
				waitTime = 10000;
			Sleep(waitTime);
			if (!QueryStatus(ssp))
			{
				break;
			}
			if (ssp.dwCheckPoint > oldCheckPoint)
			{
				startTickCount = GetTickCount();
				oldCheckPoint = ssp.dwCheckPoint;
			}
			else
			{
				if(GetTickCount() - startTickCount > ssp.dwWaitHint)
				{
					break;
				}
			}
		}
		if (!QueryStatus(ssp))
		{
			return false;
		}
		if (ssp.dwCurrentState == SERVICE_RUNNING)
		{
			return true;
		}
	}
	return false;
}

bool ServiceManager::Stop()
{
	if(NULL == m_scManager)
	{
		return false;
	}
	OpenService();
	if(m_service)
	{
		SERVICE_STATUS_PROCESS ssp = {0};
		SERVICE_STATUS ss;
		DWORD oldCheckPoint;
		DWORD startTickCount;
		DWORD waitTime;
		// Check the status until the service is no longer start pending.
		if (!QueryStatus(ssp))
		{
			return false;
		}
		
		if(ssp.dwCurrentState == SERVICE_STOPPED ) {
			return true;
		}

		if(!::ControlService(m_service, SERVICE_CONTROL_STOP, &ss))
		{
			return false;
		}
		startTickCount = GetTickCount();
		oldCheckPoint = ssp.dwCheckPoint;
		while (ssp.dwCurrentState == SERVICE_STOP_PENDING)
		{
			waitTime = ssp.dwWaitHint / 10;
			if( waitTime < 1000 )
				waitTime = 1000;
			else if ( waitTime > 10000 )
				waitTime = 10000;

			Sleep(waitTime);
			
			if (!QueryStatus(ssp))
				break;

			if (ssp.dwCheckPoint > oldCheckPoint)
			{
				startTickCount = GetTickCount();
				oldCheckPoint = ssp.dwCheckPoint;
			}
			else
			{
				if(GetTickCount() - startTickCount > ssp.dwWaitHint)
				{
					break;
				}
			}
		}

		if (!QueryStatus(ssp))
		{
			return false;
		}

		if (ssp.dwCurrentState == SERVICE_STOPPED)
		{
			return true;
		}
	}
	return false;
}

bool ServiceManager::Install(const std::string& descr, const std::string& binPath)
{
	if(NULL == m_scManager)
	{
		return false;
	}

	SC_HANDLE sh = ::CreateServiceA(m_scManager, m_serviceName.c_str(), descr.c_str()
	                                , SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START
	                                , SERVICE_ERROR_NORMAL, binPath.c_str(), NULL, NULL, NULL, NULL, NULL);
	
	if(NULL != sh)
	{
		::CloseServiceHandle(sh);
		return true;
	}

	return false;
}

bool ServiceManager::Remove()
{
	if(NULL == m_scManager)
	{
		return false;
	}

	OpenService();
	if(m_service) {
		BOOL res = ::DeleteService(m_service);
		::CloseServiceHandle(m_service);
		return res == TRUE;
	}
	return false;
}

bool ServiceManager::OpenService()
{
	if(NULL == m_scManager)
	{
		return false;
	}

	m_service = ::OpenServiceA(m_scManager, m_serviceName.c_str(), SERVICE_ALL_ACCESS);
	if(NULL == m_service)
	{
		return false;
	}

	return true;
}

bool ServiceManager::QueryStatus(SERVICE_STATUS_PROCESS& ssp)
{
	DWORD bytes;
	BOOL re = ::QueryServiceStatusEx(m_service, SC_STATUS_PROCESS_INFO,
		(LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &bytes);
	return re == TRUE;
}

#endif/*WIN32*/