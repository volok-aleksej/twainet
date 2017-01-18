#ifndef IPC_OBJECT_NAME_H
#define IPC_OBJECT_NAME_H

#include <WString.h>
#include "ipc.pb-c.h"

class IPCObjectName
{
public:
	IPCObjectName(const String& moduleName = "", const String& hostName = "", const String& connId = "");
	IPCObjectName(const Ipc__IPCName& ipcName);
	~IPCObjectName();

    bool operator == (const IPCObjectName& object) const;
    bool operator < (const IPCObjectName& object) const;
    
	bool operator == (const Ipc__IPCName& object) const;
	bool operator < (const Ipc__IPCName& object) const;
    
    void operator = (const Ipc__IPCName& object);

	String GetModuleNameString() const;
    String GetModuleName() const;
    String GetHostName() const;
    String GetConnId() const;
    
	static IPCObjectName GetIPCName(const String& ipcName);
protected:
	void ModuleNameString();
private:
	String m_moduleNameString;
    String m_moduleName;
    String m_hostName;
    String m_connId;
};

#endif/*IPC_OBJECT_NAME_H*/