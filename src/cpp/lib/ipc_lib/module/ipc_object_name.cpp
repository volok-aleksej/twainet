#include "ipc_object_name.h"
#include "utils/utils.h"

IPCObjectName::IPCObjectName(const std::string& ipcName, const std::string& hostName, const std::string& connId)
{
	set_module_name(ipcName);
	set_host_name(hostName);
	set_conn_id(connId);
	m_moduleNameString = GetModuleNameString();
}

IPCObjectName::IPCObjectName(const IPCName& ipcName)
: IPCName(ipcName)
{
	m_moduleNameString = GetModuleNameString();
}

IPCObjectName::~IPCObjectName()
{
}

bool IPCObjectName::operator == (const IPCName& ipcName) const
{
	return	module_name() == ipcName.module_name() &&
		host_name() == ipcName.host_name() &&
		conn_id() == ipcName.conn_id();
}

bool IPCObjectName::operator < (const IPCName& ipcName) const
{
	if (module_name() < ipcName.module_name() ||
		module_name() == ipcName.module_name() && host_name() < ipcName.host_name() ||
		module_name() == ipcName.module_name() && host_name() == ipcName.host_name() && conn_id() < ipcName.conn_id())
		return true;
	else
		return false;
}

std::string IPCObjectName::GetModuleNameString() const
{
	std::string moduleNameString = module_name();
	if(!host_name().empty())
	{
		moduleNameString.append(".");
		moduleNameString.append(host_name());
	}
	if(!conn_id().empty())
	{
		moduleNameString.append(".");
		moduleNameString.append(conn_id());
	}

	return moduleNameString;
}

IPCObjectName IPCObjectName::GetIPCName(const std::string& ipcName)
{
	IPCObjectName name("");
	name.m_moduleNameString = ipcName;
	std::vector<std::string> strings = CommonUtils::DelimitString(ipcName, ".");
	if(!strings.empty())
	{
		name.set_module_name(strings[0]);
	}
	if(strings.size() > 1)
	{
		name.set_host_name(strings[1]);
	}
	if(strings.size() > 2)
	{
		name.set_conn_id(strings[2]);
	}

	return name;
}
