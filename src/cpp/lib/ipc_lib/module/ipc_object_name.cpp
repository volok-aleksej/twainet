#include "ipc_object_name.h"
#include "utils/utils.h"

IPCObjectName::IPCObjectName(const std::string& ipcName, const std::string& hostName, const std::string& suffix)
{
	set_module_name(ipcName);
	set_host_name(hostName);
	set_suffix(suffix);
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
		suffix() == ipcName.suffix();
}

bool IPCObjectName::operator < (const IPCName& ipcName) const
{
	if (module_name() < ipcName.module_name() ||
		module_name() == ipcName.module_name() && host_name() < ipcName.host_name() ||
		module_name() == ipcName.module_name() && host_name() == ipcName.host_name() && suffix() < ipcName.suffix())
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
	if(!suffix().empty())
	{
		m_moduleNameString.append(".");
		m_moduleNameString.append(suffix());
	}

	return m_moduleNameString;
}

IPCObjectName IPCObjectName::GetIPCName(const std::string& ipcName)
{
	IPCObjectName name("");
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
		name.set_suffix(strings[2]);
	}

	return name;
}
