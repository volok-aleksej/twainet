#ifndef IPC_OBJECT_NAME_H
#define IPC_OBJECT_NAME_H

#pragma warning(disable:4244 4267)
#include "../messages/ipc.pb.h"
using namespace ipc;
#pragma warning(default:4244 4267)

class IPCObjectName : public IPCName
{
public:
	IPCObjectName(const std::string& ipcName, const std::string& hostName = "", const std::string& suffix = "");
	IPCObjectName(const IPCName& ipcName);
	~IPCObjectName();

	bool operator == (const IPCName& object);

	std::string GetModuleNameString() const;

	static IPCObjectName GetIPCName(const std::string& ipcName);
};

#endif/*IPC_OBJECT_NAME_H*/