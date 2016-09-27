#include <stdio.h>
#include <WString.h>

#include "ipc_connector.h"
#include "proto_message.h"
#include "ipc.pb-c.h"
#include "common_func.h"
//#include "module/ipc_module.h"

//#include "utils/logger.h"
//#include "utils/utils.h"

//#include "thread_lib/thread/thread_manager.h"

#define MAX_DATA_LEN		1024*1024

char *IPCConnector::baseAccessId = "root";

IPCConnector::IPCConnector(AnySocket* socket, const IPCObjectName& moduleName)
: Connector(socket), m_handler(this), m_moduleName(moduleName)
, m_bConnected(false), m_isExist(false)
, m_rand(CreateGUID()), m_isSendIPCObjects(false)
, m_isCoordinator(false), m_isNotifyRemove(false)
, m_checker(0)
{
	m_ipcSignal = new Signal(static_cast<SignalOwner*>(this));
	addMessage(new ModuleNameMessage(&m_handler, ipc__module_name__descriptor));
	addMessage(new AddIPCObjectMessage(&m_handler, ipc__add_ipcobject__descriptor));
	addMessage(new RemoveIPCObjectMessage(&m_handler, ipc__remove_ipcobject__descriptor));
	addMessage(new IPCProtoMessage(&m_handler, ipc__ipcmessage__descriptor));
	addMessage(new IPCObjectListMessage(&m_handler, ipc__ipcobject_list__descriptor));
	addMessage(new ChangeIPCNameMessage(&m_handler, ipc__change_ipcname__descriptor));
	addMessage(new UpdateIPCObjectMessage(&m_handler, ipc__update_ipcobject__descriptor));
	addMessage(new ModuleStateMessage(&m_handler, ipc__module_state__descriptor));
	addMessage(new PingMessage(&m_handler, ipc__ping__descriptor));
}

IPCConnector::~IPCConnector()
{
	m_ipcSignal->removeOwner();
	removeReceiver();
}

void IPCConnector::ThreadFunc()
{
	static int len = 0;
    if(!len)
    {
        if (!m_socket->Recv((char*)&len, sizeof(int)))
        {
            return;
        }
    }

	if(len < 0 || len > MAX_DATA_LEN)
	{
		return;
        len = 0;
	}

	char* data = (char*)malloc(len);
	if(!m_socket->Recv(data, len))
	{
        free(data);
		return;
	}
	
	onData(data, len);
    free(data);
    len = 0;
}

void IPCConnector::OnStart()
{
  m_checker = new IPCCheckerThread(this);
 
//  ListenerParamMessage msg(m_moduleName.GetModuleNameString());
//  onSignal(msg);
//  m_moduleName = IPCObjectName::GetIPCName(msg.m_moduleName);
//  m_isCoordinator = msg.m_isCoordinator;
    m_isNotifyRemove = m_isCoordinator;
    m_isSendIPCObjects = m_isCoordinator;
    SetAccessId(baseAccessId);

    Ipc__IPCName* ipcName = new Ipc__IPCName;
    ipcName->module_name = (char*)m_moduleName.GetModuleName().c_str();
    ipcName->host_name = (char*)m_moduleName.GetHostName().c_str();
    ipcName->conn_id = (char*)m_moduleName.GetConnId().c_str();
    
    String ip;
    int port;
    m_socket->GetIPPort(ip, port);
    
    ModuleNameMessage mnMsg(&m_handler, ipc__module_name__descriptor);
    mnMsg.GetMessage()->ipc_name = ipcName;
    mnMsg.GetMessage()->ip = (char*)ip.c_str();
    mnMsg.GetMessage()->port = port;
    mnMsg.GetMessage()->access_id = (char*)GetAccessId().c_str();
    toMessage(mnMsg);
}

void IPCConnector::OnStop()
{
    if(m_bConnected)
 	{
 		OnDisconnected();
 	}
 
 	if(m_checker)
 	{
 		m_checker->StopThread();
 		m_checker = 0;
 	}
}

void IPCConnector::SubscribeConnector(const IPCConnector* connector)
{
    IPCConnector* conn = const_cast<IPCConnector*>(connector);
    if(conn)
    {
//                ipcSubscribe(conn, SIGNAL_FUNC(this, IPCConnector, IPCProtoMessage, onIPCMessage));
        ipcSubscribe(conn, SIGNAL_FUNC(this, IPCConnector, ModuleNameMessage, onModuleNameMessage));
//                 ipcSubscribe(conn, SIGNAL_FUNC(this, IPCConnector, UpdateIPCObjectMessage, onUpdateIPCObjectMessage));
//                 ipcSubscribe(conn, SIGNAL_FUNC(this, IPCConnector, ModuleStateMessage, onModuleStateMessage));
//                 ipcSubscribe(conn, SIGNAL_FUNC(this, IPCConnector, RemoveIPCObjectMessage, onRemoveIPCObjectMessage));
    }
}

void IPCConnector::SubscribeModule(::SignalOwner* owner)
{
//	owner->addSubscriber(this, SIGNAL_FUNC(this, IPCConnector, ChangeIPCNameMessage, onChangeIPCNameMessage));
//	owner->addSubscriber(this, SIGNAL_FUNC(this, IPCConnector, IPCMessageSignal, onIPCMessage));
}

IPCObjectName IPCConnector::GetModuleName() const
{
	return m_moduleName;
}

void IPCConnector::SetAccessId(const String& accessId)
{
	m_accessId = accessId;
}

String IPCConnector::GetAccessId()
{
	return m_accessId;
}
/*	
void IPCConnector::onIPCMessage(const IPCProtoMessage& msg)
{
	if(msg.ipc_path_size() == 0)
	{
		return;
	}

	IPCObjectName path(msg.ipc_path(0));
	if(path.GetModuleNameString() == m_id)
	{
		toMessage(msg);
	}
}*/

bool IPCConnector::SendData(char* data, int len)
{
	char* senddata = (char*)malloc(len + sizeof(int));
	memcpy(senddata + sizeof(int), data, len);
	memcpy(senddata, &len, sizeof(int));
	bool ret = Connector::SendData(senddata, len + sizeof(int));
    free(senddata);
    return ret;
}
/*
void IPCConnector::onUpdateIPCObjectMessage(const UpdateIPCObjectMessage& msg)
{
	toMessage(msg);
}

void IPCConnector::onChangeIPCNameMessage(const ChangeIPCNameMessage& msg)
{
	if(SetModuleName(msg.ipc_name()))
	{
		toMessage(msg);
	}
}

void IPCConnector::onRemoveIPCObjectMessage(const RemoveIPCObjectMessage& msg)
{
	toMessage(msg);
}

void IPCConnector::onIPCMessage(const IPCMessageSignal& msg)
{
	IPCObjectName ipcName("");
	if(msg.ipc_path_size() != 0)
	{
		ipcName = const_cast<IPCMessageSignal&>(msg).ipc_path(0);
	}

	if (msg.ipc_path_size() == 0 ||
		ipcName == IPCObjectName::GetIPCName(GetId()))
	{
		IPCProtoMessage ipcMsg(&m_handler, static_cast<const IPCMessage&>(msg));
		toMessage(ipcMsg);
	}
}
*/
void IPCConnector::onModuleNameMessage(const ModuleNameMessage& msg)
{
	if(m_moduleName == *const_cast<ModuleNameMessage&>(msg).GetMessage()->ipc_name)
	{
		return;
	}

	AddIPCObjectMessage aoMsg(&m_handler, ipc__add_ipcobject__descriptor);
	aoMsg.GetMessage()->ip = const_cast<ModuleNameMessage&>(msg).GetMessage()->ip;
	aoMsg.GetMessage()->port = const_cast<ModuleNameMessage&>(msg).GetMessage()->port;
	aoMsg.GetMessage()->access_id = const_cast<ModuleNameMessage&>(msg).GetMessage()->access_id;
	aoMsg.GetMessage()->ipc_name = const_cast<ModuleNameMessage&>(msg).GetMessage()->ipc_name;
	toMessage(aoMsg);
}

void IPCConnector::onModuleStateMessage(const ModuleStateMessage& msg)
{
	if(m_id == const_cast<ModuleStateMessage&>(msg).GetMessage()->id)
	{
		const_cast<ModuleStateMessage&>(msg).GetMessage()->exist = true;
		if(strcmp(m_rand.c_str(), const_cast<ModuleStateMessage&>(msg).GetMessage()->rndval) < 0)
		{
			const_cast<ModuleStateMessage&>(msg).GetMessage()->rndval = (char*)m_rand.c_str();
		}
	}
}

void IPCConnector::addIPCSubscriber(SignalReceiver* receiver, IReceiverFunc* func)
{
	receiver->addSignal(m_ipcSignal, func);
}

void IPCConnector::onNewConnector(const Connector* connector)
{
        IPCConnector* conn = const_cast<IPCConnector*>(static_cast<const IPCConnector*>(connector));
        if(conn)
        {
                SubscribeConnector(conn);
                conn->SubscribeConnector(this);
        }
}

void IPCConnector::ipcSubscribe(IPCConnector* connector, IReceiverFunc* func)
{
	connector->addIPCSubscriber(this, func);
}

void IPCConnector::onIPCSignal(const DataMessage& msg)
{
	m_ipcSignal->onSignal(msg);
}

bool IPCConnector::SetModuleName(const IPCObjectName& moduleName)
{
//	LOG_INFO("Set module name: old-%s, new-%s\n", m_moduleName.GetModuleNameString().c_str(), const_cast<IPCObjectName&>(moduleName).GetModuleNameString().c_str());
	m_moduleName = moduleName;
	return true;
}

void IPCConnector::OnConnected()
{
 	m_bConnected = true;
// 	ConnectedMessage msg(GetId(), !m_isCoordinator);
// 	onSignal(msg);
}

void IPCConnector::OnDisconnected()
{
	m_bConnected = false;
}

void IPCConnector::OnAddIPCObject(const String& moduleName)
{
}

void IPCConnector::OnUpdateIPCObject(const String& oldModuleName, const String& newModuleName)
{
}

IPCObjectName IPCConnector::GetIPCName()
{
	return IPCObjectName::GetIPCName(GetId());
}

