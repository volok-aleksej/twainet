#ifndef CLIENT_MODULE_NAME_H
#define CLIENT_MODULE_NAME_H

#include <string>

class ClientModuleName
{
public:
	ClientModuleName(){}
	ClientModuleName(const ClientModuleName& name)
	{
		operator = (name);
	}
	ClientModuleName(const std::string& moduleName, const std::string& hostClient)
	: m_moduleName(moduleName), m_hostClient(hostClient){}
	~ClientModuleName(){}
	
	void operator = (const ClientModuleName& name)
	{
		m_moduleName = name.m_moduleName;
		m_hostClient = name.m_hostClient;
	}
	
	bool operator == (const ClientModuleName& name) const
	{
		return m_hostClient == name.m_hostClient;
	}
	
	bool operator != (const ClientModuleName& name) const
	{
	  	return !operator == (name);
	}
	
	bool operator < (const ClientModuleName& name) const
	{
		return m_hostClient < name.m_hostClient;
	}
	
public:
	std::string m_moduleName;
	std::string m_hostClient;
};

#endif/*CLIENT_MODULE_NAME_H*/