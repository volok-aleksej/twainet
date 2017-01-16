#include "ipc_handler.h"
#include "ipc_connector.h"
//#include "utils/logger.h"
//#include "include/ref.h"
//#include "module/ipc_module.h"

template<> String IPCNameMessage::messageName = ipc__ipcname__descriptor.name;
template<> String IPCProtoMessage::messageName = ipc__ipcmessage__descriptor.name;
template<> String ModuleNameMessage::messageName = ipc__module_name__descriptor.name;
template<> String AddIPCObjectMessage::messageName = ipc__add_ipcobject__descriptor.name;
template<> String UpdateIPCObjectMessage::messageName = ipc__update_ipcobject__descriptor.name;
template<> String ChangeIPCNameMessage::messageName = ipc__change_ipcname__descriptor.name;
template<> String RemoveIPCObjectMessage::messageName = ipc__remove_ipcobject__descriptor.name;
template<> String IPCObjectListMessage::messageName = ipc__ipcobject_list__descriptor.name;
template<> String ModuleStateMessage::messageName = ipc__module_state__descriptor.name;
template<> String PingMessage::messageName = ipc__ping__descriptor.name;

template<> String IPCSignalMessage::messageName = ipc__ipcmessage__descriptor.name;

IPCHandler::IPCHandler(IPCConnector* connector)
: m_connector(connector)
{
}

IPCHandler::~IPCHandler()
{
}
	
void IPCHandler::onMessage(const _Ipc__ModuleName& msg)
{
 	if(m_connector->m_checker)
 	{
 		m_connector->m_checker->StopThread();
 		m_connector->m_checker = 0;
 	}
	
 	IPCObjectName ipcName(*msg.ipc_name);
 	m_connector->SetId(ipcName.GetModuleNameString());

// 	LOG_INFO("ModuleName message: m_id-%s, m_module-%s\n",
// 		 m_connector->m_id.c_str(), m_connector->m_moduleName.GetModuleNameString().c_str());
 
 	AddIPCObjectMessage aoMsg(this, ipc__add_ipcobject__descriptor);
    aoMsg.GetMessage()->ip = msg.ip;
    aoMsg.GetMessage()->port = msg.port;
    aoMsg.GetMessage()->access_id = IPCConnector::baseAccessId;
    aoMsg.GetMessage()->ipc_name = msg.ipc_name;
 	m_connector->onSignal(aoMsg);

 	ModuleNameMessage mnMsg(this, ipc__module_name__descriptor);
    mnMsg.GetMessage()->ip = msg.ip;
    mnMsg.GetMessage()->port = msg.port;
    mnMsg.GetMessage()->ipc_name = msg.ipc_name;
 	mnMsg.GetMessage()->is_exist = false;
 	mnMsg.GetMessage()->conn_id = (char*)m_connector->m_connectorId.c_str();
 	mnMsg.GetMessage()->access_id == IPCConnector::baseAccessId;
 	m_connector->onSignal(mnMsg);
 
 	ModuleStateMessage msMsg(this, ipc__module_state__descriptor);
 	msMsg.GetMessage()->exist = m_connector->m_isExist;
 	msMsg.GetMessage()->rndval = (char*)m_connector->m_rand.c_str();
 	m_connector->toMessage(msMsg);
 
 	if(m_connector->m_isSendIPCObjects)
 	{
 		IPCObjectListMessage ipcolMsg(this, ipc__ipcobject_list__descriptor);
 		ipcolMsg.GetMessage()->access_id = msg.access_id;
 		m_connector->onSignal(ipcolMsg);
 		m_connector->toMessage(ipcolMsg);
 	}
 	
 	if(m_connector->m_isExist)
 	{
// 		LOG_INFO("Module exists: m_id-%s, m_module-%s\n", m_connector->m_id.c_str(), m_connector->m_moduleName.GetModuleNameString().c_str());
 		return;
 	}
 
 	//TODO: fix problem:
 	//	if two modules with same names is trying to connect in same time to coordinator,
 	//	another modules cann't get correct creadentials of new module.
 	if(m_connector->m_isSendIPCObjects)
 	{
 		m_connector->onIPCSignal(mnMsg);
 	}
 	m_connector->OnConnected();
}

void IPCHandler::onMessage(const _Ipc__ModuleState& msg)
{
 	if(msg.exist && m_connector->m_isExist)
 	{
// 		LOG_INFO("Module exists: m_id-%s, m_module-%s\n",
// 			 m_connector->m_id.c_str(), m_connector->m_moduleName.GetModuleNameString().c_str());
 		m_connector->StopThread();
 	}
 	else if(msg.exist && !m_connector->m_isExist)
 	{
 		ModuleStateMessage msMsg(this, ipc__module_state__descriptor);
 		msMsg.GetMessage()->exist = false;
 		msMsg.GetMessage()->id = (char*)m_connector->GetId().c_str();
 		m_connector->onIPCSignal(msMsg);
 
 		if (msMsg.GetMessage()->exist && m_connector->m_rand > msMsg.GetMessage()->rndval
 			|| !msMsg.GetMessage()->exist)
 		{
// 			LOG_INFO("Module exists: m_id-%s, m_module-%s\n",
// 				 m_connector->m_id.c_str(), m_connector->m_moduleName.GetModuleNameString().c_str());
 			m_connector->StopThread();
 		}
 		else if(msMsg.GetMessage()->exist && m_connector->m_rand == msMsg.GetMessage()->rndval)
 		{
// 			LOG_WARNING("Random values is equal: m_id-%s, m_module-%s\n",
// 				    m_connector->m_id.c_str(), m_connector->m_moduleName.GetModuleNameString().c_str());
 		}
 		else
 		{
 			m_connector->OnConnected();
 		}
 	}
}

void IPCHandler::onMessage(const _Ipc__AddIPCObject& msg)
{
 	AddIPCObjectMessage aoMsg(this, ipc__add_ipcobject__descriptor);
	aoMsg.GetMessage()->access_id = msg.access_id;
	aoMsg.GetMessage()->ip = msg.ip;
	aoMsg.GetMessage()->port = msg.port;
	aoMsg.GetMessage()->ipc_name = msg.ipc_name;
 	m_connector->onSignal(aoMsg);
 
 	IPCObjectName ipcName(*msg.ipc_name);
 	m_connector->OnAddIPCObject(ipcName.GetModuleNameString());
}

void IPCHandler::onMessage(const _Ipc__RemoveIPCObject& msg)
{
 	RemoveIPCObjectMessage roMsg(this, ipc__remove_ipcobject__descriptor);
	roMsg.GetMessage()->ipc_name = msg.ipc_name;
 	m_connector->onSignal(roMsg);
 }

void IPCHandler::onMessage(const _Ipc__IPCMessage& msg)
{
 	if(!m_connector->m_bConnected)
 	{
 		return;
 	}
 	
 	Ipc__IPCName **namesPath = 0, **namesSender = 0;
 	bool isTarget = (msg.n_ipc_path == 0);
 
 	IPCProtoMessage newMsg(this, ipc__ipcmessage__descriptor);
    newMsg.GetMessage()->has_message = msg.has_message;
    newMsg.GetMessage()->message = msg.message;
    newMsg.GetMessage()->ipc_sender = msg.ipc_sender;
    newMsg.GetMessage()->n_ipc_sender = msg.n_ipc_sender;
    newMsg.GetMessage()->message_name = msg.message_name;        
 	if(!isTarget)
 	{
 		IPCObjectName newPath(*msg.ipc_path[0]);
 		if(newPath == m_connector->m_moduleName && msg.n_ipc_path > 1)
 		{
            newMsg.GetMessage()->n_ipc_path = msg.n_ipc_path - 1;
            namesPath = (Ipc__IPCName**)malloc(sizeof(Ipc__IPCName*) * (msg.n_ipc_path - 1));
            newMsg.GetMessage()->ipc_path = namesPath;
 			for(int i = 1; i < msg.n_ipc_path; i++)
 			{
 				newMsg.GetMessage()->ipc_path[i - 1] =  msg.ipc_path[i];
 			}
 		}
        
        newMsg.GetMessage()->n_ipc_sender = msg.n_ipc_sender + 1;
        namesSender = (Ipc__IPCName**)malloc(sizeof(Ipc__IPCName*) * (msg.n_ipc_sender + 1));
        newMsg.GetMessage()->ipc_sender = namesSender;
        for(int i = 0; i < msg.n_ipc_sender; i++)
        {
            newMsg.GetMessage()->ipc_sender[i] =  msg.ipc_sender[i];
        }
        
        IPCNameMessage name(this, ipc__ipcname__descriptor);
        name.GetMessage()->module_name = (char*)m_connector->GetIPCName().GetModuleName().c_str();
        name.GetMessage()->host_name = (char*)m_connector->GetIPCName().GetHostName().c_str();
        name.GetMessage()->conn_id = (char*)m_connector->GetIPCName().GetConnId().c_str();
        newMsg.GetMessage()->ipc_sender[msg.n_ipc_sender] = name.GetMessage();
 		isTarget = (newPath == m_connector->m_moduleName && !newMsg.GetMessage()->n_ipc_path);
 	}
 
 	if (isTarget &&
 	    !m_connector->onData(msg.message_name, (char*)msg.message.data, (int)msg.message.len))
 	{
 		m_connector->onSignal(newMsg);
 	}
 	else if(newMsg.GetMessage()->n_ipc_path)
 	{
 		IPCSignalMessage sigMsg(*newMsg.GetMessage());
 		m_connector->onSignal(sigMsg);
 		m_connector->onIPCSignal(newMsg);
 	}
 	
 	if(namesPath)
    {
        free(namesPath);
    }
    if(namesSender)
    {
        free(namesSender);
    }
}

void IPCHandler::onMessage(const _Ipc__IPCObjectList& msg)
{
 	for(int i = 0; i < msg.n_ipc_object; i++)
 	{
 		AddIPCObjectMessage aoMsg(this, ipc__add_ipcobject__descriptor);
        aoMsg.GetMessage()->access_id = msg.ipc_object[i]->access_id;
        aoMsg.GetMessage()->ip = msg.ipc_object[i]->ip;
        aoMsg.GetMessage()->port = msg.ipc_object[i]->port;
        aoMsg.GetMessage()->ipc_name = msg.ipc_object[i]->ipc_name;
 		m_connector->onSignal(aoMsg);
 		
 		IPCObjectName ipcName(*msg.ipc_object[i]->ipc_name);
 		m_connector->OnAddIPCObject(ipcName.GetModuleNameString());
 	}
}

void IPCHandler::onMessage(const _Ipc__ChangeIPCName& msg)
{
    IPCObjectName objectName = IPCObjectName::GetIPCName(m_connector->m_id);
    IPCNameMessage inMsg(this, ipc__ipcname__descriptor);
    inMsg.GetMessage()->module_name = (char*)objectName.GetModuleName().c_str();
    inMsg.GetMessage()->host_name = (char*)objectName.GetHostName().c_str();
    inMsg.GetMessage()->conn_id = (char*)objectName.GetConnId().c_str();
 	UpdateIPCObjectMessage uioMsg(this, ipc__update_ipcobject__descriptor);
    uioMsg.GetMessage()->ipc_new_name = msg.ipc_name;
    uioMsg.GetMessage()->ipc_old_name = inMsg.GetMessage();
 	m_connector->onSignal(uioMsg);
	m_connector->onIPCSignal(uioMsg);
 
 	IPCObjectName ipcName(*msg.ipc_name);
 	m_connector->m_id = ipcName.GetModuleNameString();
 	
// 	LOG_INFO("Change connector id: m_id-%s, m_module-%s\n", m_connector->m_id.c_str(), m_connector->m_moduleName.GetModuleNameString().c_str());
}

void IPCHandler::onMessage(const _Ipc__UpdateIPCObject& msg)
{
 	UpdateIPCObjectMessage uioMsg(this, ipc__update_ipcobject__descriptor);
    uioMsg.GetMessage()->ipc_new_name = msg.ipc_new_name;
    uioMsg.GetMessage()->ipc_old_name = msg.ipc_old_name;
 	m_connector->onSignal(uioMsg);
 
 	IPCObjectName ipcNameOld(*msg.ipc_old_name);
 	IPCObjectName ipcNameNew(*msg.ipc_new_name);
 	m_connector->OnUpdateIPCObject(ipcNameOld.GetModuleNameString(), ipcNameNew.GetModuleNameString());
}

void IPCHandler::onMessage(const _Ipc__Ping& msg)
{
}

void IPCHandler::onMessage(const _Ipc__IPCName& msg)
{
}