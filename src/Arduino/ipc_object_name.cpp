#include "ipc_object_name.h"
//#include "utils/utils.h"

IPCObjectName::IPCObjectName(const String& moduleName, const String& hostName, const String& connId)
{
	m_moduleName = moduleName;
	m_hostName = hostName;
	m_connId = connId;
	m_moduleNameString = GetModuleNameString();
}

IPCObjectName::IPCObjectName(const Ipc__IPCName& ipcName)
: m_moduleName(ipcName.module_name), m_hostName(ipcName.host_name), m_connId(ipcName.conn_id)
{
	m_moduleNameString = GetModuleNameString();
}

IPCObjectName::~IPCObjectName()
{
}

bool IPCObjectName::operator == (const Ipc__IPCName& ipcName) const
{
	return	m_moduleName == ipcName.module_name &&
		m_hostName == ipcName.host_name &&
		m_connId == ipcName.conn_id;
}

bool IPCObjectName::operator < (const Ipc__IPCName& ipcName) const
{
	if (m_moduleName < ipcName.module_name ||
		m_moduleName == ipcName.module_name && m_hostName < ipcName.host_name ||
		m_moduleName == ipcName.module_name && m_hostName == ipcName.host_name && m_connId < ipcName.conn_id)
		return true;
	else
		return false;
}

String IPCObjectName::GetModuleNameString() const
{
	String moduleNameString = m_moduleName;
	if(m_hostName.length())
	{
		moduleNameString += ".";
		moduleNameString += m_hostName.c_str();
	}
	if(m_connId.length())
	{
		moduleNameString += ".";
		moduleNameString += m_connId.c_str();
	}

	return moduleNameString;
}

String IPCObjectName::GetModuleName() const
{
    return m_moduleName;
}

String IPCObjectName::GetHostName() const
{
    return m_hostName;
}

String IPCObjectName::GetConnId() const
{
    return m_connId;
}
    
IPCObjectName IPCObjectName::GetIPCName(const String& ipcName)
{
	IPCObjectName name("");

    
	return name;
}
