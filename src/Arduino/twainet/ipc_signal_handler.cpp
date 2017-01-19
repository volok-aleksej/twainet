#include <stdio.h>
#include <time.h>
#include "ipc_signal_handler.h"
#include "ipc_module.h"
#include "thread_manager.h"

IPCSignalHandler::IPCSignalHandler(IPCModule* module)
: m_module(module)
{
}

IPCSignalHandler::~IPCSignalHandler()
{
	removeReceiver();
}

void IPCSignalHandler::onAddConnector(const ConnectorMessage& msg)
{
	m_module->AddConnector(msg.m_conn);
}

void IPCSignalHandler::onConnected(const ConnectedMessage& msg)
{
	m_module->OnConnected(msg.m_id);
}

void IPCSignalHandler::onIPCMessage(const IPCProtoMessage& msg)
{
// 	std::vector<std::string> path;
// 	for(int i = 0; i < msg.ipc_sender_size(); i++)
// 	{
// 		IPCObjectName sender(msg.ipc_sender(i));
// 		path.push_back(sender.GetModuleNameString());
// 	}
// 	m_module->OnMessage(msg.message_name(), path, msg.message());
}

void IPCSignalHandler::onIPCMessage(const IPCSignalMessage& msg)
{/*
	IPCObjectName newPath(msg.ipc_path(0));
	std::vector<IPCObjectName> path = m_module->GetTargetPath(newPath);
	if(path.size() > 1)
	{
		const_cast<IPCMessageSignal&>(msg).clear_ipc_path();
		for(std::vector<IPCObjectName>::iterator it = path.begin();
		    it != path.end(); it++)
		{
		    *const_cast<IPCMessageSignal&>(msg).add_ipc_path() = *it;
		}
	}
	else if(path.size() == 0 && m_module->GetModuleName() == newPath)
	{
		IPCProtoMessage protoMsg(0, msg);
		onIPCMessage(protoMsg);
	}*/
}

void IPCSignalHandler::onAddIPCObject(const AddIPCObjectMessage& msg)
{
	IPCModule::IPCObject object(*const_cast<AddIPCObjectMessage&>(msg).GetMessage()->ipc_name,
                                const_cast<AddIPCObjectMessage&>(msg).GetMessage()->ip,
                                const_cast<AddIPCObjectMessage&>(msg).GetMessage()->port,
                                const_cast<AddIPCObjectMessage&>(msg).GetMessage()->access_id);
// 	LOG_INFO("Add IPC Object: ipc name - %s, m_moduleName - %s\n",
// 		 object.m_ipcName.GetModuleNameString().c_str(), m_module->m_moduleName.GetModuleNameString().c_str());
	m_module->m_ipcObject.insert(m_module->m_ipcObject.end(), object);
	m_module->OnIPCObjectsChanged();
}

void IPCSignalHandler::onUpdateIPCObject(const UpdateIPCObjectMessage& msg)
{
	IPCModule::IPCObject object(*const_cast<UpdateIPCObjectMessage&>(msg).GetMessage()->ipc_old_name);
    for(twnstd::list<IPCModule::IPCObject>::iterator it = m_module->m_ipcObject.begin();
        it != m_module->m_ipcObject.end(); ++it)
    {
        if(*it == object)
        {
            *it = IPCModule::IPCObject(*const_cast<UpdateIPCObjectMessage&>(msg).GetMessage()->ipc_new_name);
            break;
        }
    }
    bool update = false;
    for(twnstd::list<IPCModule::IPCObject>::iterator it = m_module->m_modules.begin();
        it != m_module->m_modules.end(); ++it)
    {
        if(*it == object)
        {
            update = true;
            *it = IPCModule::IPCObject(*const_cast<UpdateIPCObjectMessage&>(msg).GetMessage()->ipc_new_name);
            break;
        }
    }
    
	m_module->OnIPCObjectsChanged();
/*
	IPCObjectName ipcNameNew(msg.ipc_new_name());
	IPCObjectName ipcNameOld(msg.ipc_old_name());
	LOG_INFO("Update IPC Object: ipc name old - %s, ipc name new - %s, m_moduleName - %s\n",
			ipcNameOld.GetModuleNameString().c_str(),
			ipcNameNew.GetModuleNameString().c_str(),
			m_module->m_moduleName.GetModuleNameString().c_str());*/
}

void IPCSignalHandler::onRemoveIPCObject(const RemoveIPCObjectMessage& msg)
{
    IPCObjectName ipcName = IPCObjectName::GetIPCName(const_cast<RemoveIPCObjectMessage&>(msg).GetMessage()->ipc_name);
    for(twnstd::list<IPCModule::IPCObject>::iterator it = m_module->m_ipcObject.begin();
        it != m_module->m_ipcObject.end(); ++it)
    {
        if(it->m_ipcName == ipcName)
        {
            m_module->m_ipcObject.erase(it);
            break;
        }
    }
    
    m_module->OnIPCObjectsChanged();
// 	LOG_INFO("Remove IPC Object: ipc name - %s, m_moduleName - %s\n",
// 		 object.m_ipcName.GetModuleNameString().c_str(), m_module->m_moduleName.GetModuleNameString().c_str());
}

void IPCSignalHandler::onModuleName(const ModuleNameMessage& msg)
{
 	IPCModule::IPCObject module(IPCObjectName(*const_cast<ModuleNameMessage&>(msg).GetMessage()->ipc_name),
                                const_cast<ModuleNameMessage&>(msg).GetMessage()->ip,
                                const_cast<ModuleNameMessage&>(msg).GetMessage()->port,
                                const_cast<ModuleNameMessage&>(msg).GetMessage()->access_id);
    bool isexist = false;
    for(twnstd::list<IPCModule::IPCObject>::iterator it = m_module->m_modules.begin();
        it != m_module->m_modules.end(); ++it)
    {
        if(*it == module)
        {
            isexist = true;
            break;
        }
    }
 	const_cast<ModuleNameMessage&>(msg).GetMessage()->is_exist = isexist;
}

void IPCSignalHandler::onDisconnected(const DisconnectedMessage& msg)
{
    if(!m_module->CheckFireConnector(msg.m_id))
	{
		return;
	}

	IPCModule::IPCObject module = IPCObjectName::GetIPCName(msg.m_id);
    for(twnstd::list<IPCModule::IPCObject>::iterator it = m_module->m_modules.begin();
        it != m_module->m_modules.end(); ++it)
    {
        if(*it == module)
        {
            m_module->m_modules.erase(it);
            break;
        }
    }
    
	m_module->OnFireConnector(msg.m_id);
}

void IPCSignalHandler::onIPCObjectList(const IPCObjectListMessage& msg)
{
// 	std::vector<IPCModule::IPCObject> list = m_module->m_ipcObject.GetObjectList();
// 	m_module->FillIPCObjectList(list);
// 	std::vector<IPCModule::IPCObject>::iterator it;
// 	for(it = list.begin(); it != list.end(); it++)
// 	{
// 		if(it->m_accessId != msg.access_id())
// 			continue;
// 
// 		AddIPCObject* ipcObject = const_cast<IPCObjectListMessage&>(msg).add_ipc_object();
// 		ipcObject->set_ip(it->m_ip);
// 		ipcObject->set_port(it->m_port);
// 		ipcObject->set_access_id(it->m_accessId);
// 		*ipcObject->mutable_ipc_name() = it->m_ipcName;
// 	}
}
