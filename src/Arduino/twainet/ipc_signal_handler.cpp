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
 	twnstd::vector<String> path;
 	for(int i = 0; i < const_cast<IPCProtoMessage&>(msg).GetMessage()->n_ipc_sender; i++)
 	{
 		IPCObjectName sender(*const_cast<IPCProtoMessage&>(msg).GetMessage()->ipc_sender[i]);
 		path.push_back(sender.GetModuleNameString());
 	}
 	
 	m_module->OnMessage(const_cast<IPCProtoMessage&>(msg).GetMessage()->message_name, path,
                        (const char*)const_cast<IPCProtoMessage&>(msg).GetMessage()->message.data,
                        const_cast<IPCProtoMessage&>(msg).GetMessage()->message.len);
}

void IPCSignalHandler::onIPCMessage(const IPCSignalMessage& msg)
{
	IPCObjectName newPath(*const_cast<IPCSignalMessage&>(msg).ipc_path[0]);
	twnstd::vector<IPCObjectName> path = m_module->GetTargetPath(newPath);
	if(path.length() > 1)
	{
        IPCSignalMessage tomsg(msg);
 		tomsg.n_ipc_path = path.length();
        tomsg.ipc_path = (Ipc__IPCName**)malloc(sizeof(Ipc__IPCName*) * path.length() + sizeof(Ipc__IPCName) * path.length());
 		for(unsigned int i = 0; i < path.length(); i++)
 		{
            tomsg.ipc_path[i] = (Ipc__IPCName*)(((char*)tomsg.ipc_path) + sizeof(Ipc__IPCName*) * path.length() + sizeof(Ipc__IPCName) * i);
 		    tomsg.ipc_path[i]->module_name = (char*)path[i].GetModuleName().c_str();
            tomsg.ipc_path[i]->host_name = (char*)path[i].GetHostName().c_str();
            tomsg.ipc_path[i]->conn_id = (char*)path[i].GetConnId().c_str();
 		}
 		m_module->onSignal(msg);
        free(tomsg.ipc_path);
	}
	else if(path.length() == 0 && m_module->GetModuleName() == newPath)
	{
 		IPCProtoMessage protoMsg(0, ipc__ipcmessage__descriptor);
        *protoMsg.GetMessage() = *(Ipc__IPCMessage*)(&msg);
 		onIPCMessage(protoMsg);
	}
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
 	twnstd::list<IPCModule::IPCObject> list = m_module->m_ipcObject;
 	m_module->FillIPCObjectList(list);
    const_cast<IPCObjectListMessage&>(msg).GetMessage()->n_ipc_object = list.size();
    const_cast<IPCObjectListMessage&>(msg).GetMessage()->ipc_object = 
        (Ipc__AddIPCObject**)malloc(sizeof(Ipc__AddIPCObject*) * list.size() + sizeof(Ipc__AddIPCObject) * list.size() + sizeof(Ipc__IPCName)* list.size());
    unsigned int offsetobjects = sizeof(Ipc__AddIPCObject*) * list.size();
    unsigned int offsetnames = offsetobjects + sizeof(Ipc__AddIPCObject) * list.size();
    twnstd::list<IPCModule::IPCObject>::iterator it;
    int i;
 	for(it = list.begin(), i = 0; it != list.end(); ++it, i++)
 	{
 		if(it->m_accessId != const_cast<IPCObjectListMessage&>(msg).GetMessage()->access_id)
 			continue;
 
        char* dataPtr = (char*)const_cast<IPCObjectListMessage&>(msg).GetMessage()->ipc_object;
        const_cast<IPCObjectListMessage&>(msg).GetMessage()->ipc_object[i] = (Ipc__AddIPCObject*)(dataPtr + offsetobjects + sizeof(Ipc__AddIPCObject) * i);
        Ipc__AddIPCObject* addIPCObject = const_cast<IPCObjectListMessage&>(msg).GetMessage()->ipc_object[i];
 		addIPCObject->ip = (char*)it->m_ip.c_str();
 		addIPCObject->port = it->m_port;
 		addIPCObject->access_id = (char*)it->m_accessId.c_str();
        addIPCObject->ipc_name = (Ipc__IPCName*)(dataPtr + offsetnames + sizeof(Ipc__IPCName) * i);
        addIPCObject->ipc_name->module_name = (char*)it->m_ipcName.GetModuleName().c_str();
        addIPCObject->ipc_name->host_name = (char*)it->m_ipcName.GetHostName().c_str();
        addIPCObject->ipc_name->conn_id = (char*)it->m_ipcName.GetConnId().c_str();
 	}
}
