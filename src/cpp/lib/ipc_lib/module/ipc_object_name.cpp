#include "ipc_object_name.h"
#include "utils/utils.h"

IPCObjectName::IPCObjectName(const std::string& ipcName, const std::string& hostName, const std::string& conn_id)
{
	set_module_name(ipcName);
	set_host_name(hostName);
	set_conn_id(conn_id);
}

IPCObjectName::IPCObjectName(const IPCName& ipcName)
: IPCName(ipcName)
{
}

IPCObjectName::~IPCObjectName()
{
}

bool IPCObjectName::operator == (const IPCName& ipcName)
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

const std::string& IPCObjectName::GetModuleNameString()
{
	m_moduleNameString = module_name();
	if(!host_name().empty())
	{
		m_moduleNameString.append(".");
		m_moduleNameString.append(host_name());
	}
	if(!conn_id().empty())
	{
		m_moduleNameString.append(".");
		m_moduleNameString.append(conn_id());
	}

	return m_moduleNameString;
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
