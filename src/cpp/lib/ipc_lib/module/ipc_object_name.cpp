#include "ipc_object_name.h"
#include "utils/utils.h"

IPCObjectName::IPCObjectName(const std::string& ipcName, const std::string& hostName, const std::string& suffix, const std::string& internal)
{
	set_module_name(ipcName);
	set_host_name(hostName);
	set_suffix(suffix);
	set_internal(internal);
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
		suffix() == ipcName.suffix() &&
		internal() == ipcName.internal();
}

bool IPCObjectName::operator < (const IPCName& ipcName) const
{
	if (module_name() < ipcName.module_name() ||
		module_name() == ipcName.module_name() && host_name() < ipcName.host_name() ||
		module_name() == ipcName.module_name() && host_name() == ipcName.host_name() && suffix() < ipcName.suffix() ||
		module_name() == ipcName.module_name() && host_name() == ipcName.host_name() && suffix() == ipcName.suffix() && internal() < ipcName.internal())
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
	if(!internal().empty())
	{
		m_moduleNameString.append("->");
		m_moduleNameString.append(internal());
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
		
		std::vector<std::string> suffix_internal = CommonUtils::DelimitString(strings[2], "->");
		if(!suffix_internal.empty())
		{
			name.set_suffix(suffix_internal[0]);
		}
		if(suffix_internal.size() > 1)
		{
			name.set_internal(suffix_internal[1]);
		}
	}

	return name;
}
