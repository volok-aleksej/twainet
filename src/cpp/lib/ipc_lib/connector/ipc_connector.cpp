#include <stdio.h>

#include "ipc_connector.h"
#include "internal_connector.h"
#include "module/ipc_module.h"

#include "common/common_func.h"
#include "common/logger.h"
#include "common/user.h"
#include "common/ref.h"
#include "utils/utils.h"

#include "thread_lib/thread/thread_manager.h"

#pragma warning(disable: 4355)

#define MAX_DATA_LEN		1024*1024

IPCConnector::IPCConnector(AnySocket* socket, const IPCObjectName& moduleName)
: Connector(socket), m_handler(this), m_moduleName(moduleName)
, m_bConnected(false), m_isCoordinator(false)
, m_isNotifyRemove(true), m_isSendIPCObjects(false)
, m_checker(0), m_isExist(false), m_rand(CreateGUID())
{
	m_manager = new ConnectorManager;
	m_manager->addSubscriber(this, SIGNAL_FUNC(this, IPCConnector, DisconnectedMessage, onDisconnected));
	m_ipcSignal = new Signal(static_cast<SignalOwner*>(this));
	addMessage(new ProtoMessage<ModuleName>(&m_handler));
	addMessage(new ProtoMessage<AddIPCObject>(&m_handler));
	addMessage(new ProtoMessage<RemoveIPCObject>(&m_handler));
	addMessage(new ProtoMessage<IPCMessage>(&m_handler));
	addMessage(new ProtoMessage<IPCObjectList>(&m_handler));
	addMessage(new ProtoMessage<ChangeIPCName>(&m_handler));
	addMessage(new ProtoMessage<UpdateIPCObject>(&m_handler));
	addMessage(new ProtoMessage<ModuleState>(&m_handler));
	addMessage(new ProtoMessage<Ping>(&m_handler));
		
	addMessage(new ProtoMessage<InitInternalConnection>(&m_handler));
	addMessage(new ProtoMessage<InternalConnectionStatus>(&m_handler));
	addMessage(new ProtoMessage<InternalConnectionData>(&m_handler));
}

IPCConnector::~IPCConnector()
{
	m_ipcSignal->removeOwner();
	removeReceiver();
}

void IPCConnector::ThreadFunc()
{
	int len = 0;
	while(!IsStop())
	{
		if (!m_socket->Recv((char*)&len, sizeof(int)))
		{
			break;
		}

		if(len < 0 || len > MAX_DATA_LEN)
		{
			break;
		}

		std::string data;
		data.resize(len);
		if(!m_socket->Recv((char*)data.c_str(), len))
		{
			break;
		}

		onData((char*)data.c_str(), len);
	}
}

void IPCConnector::OnStart()
{
	m_checker = new IPCCheckerThread(this);

	ListenerParamMessage msg(m_moduleName.GetModuleNameString());
	onSignal(msg);
	m_moduleName = IPCObjectName::GetIPCName(msg.m_moduleName);
	m_isCoordinator = msg.m_isCoordinator;
	m_isNotifyRemove = m_isCoordinator;
	m_isSendIPCObjects = m_isNotifyRemove;
	SetAccessId(GetUserName());

	ProtoMessage<ModuleName> mnMsg(&m_handler);
	*mnMsg.mutable_ipc_name() = m_moduleName;
	mnMsg.set_ip(msg.m_ip);
	mnMsg.set_port(msg.m_port);
	mnMsg.set_access_id(m_accessId);
	toMessage(mnMsg);
}

void IPCConnector::OnStop()
{
	if(m_bConnected)
	{
		OnDisconnected();
	}
	
	if(m_isNotifyRemove)
	{
		RemoveIPCObjectMessage msg(&m_handler);
		msg.set_ipc_name(m_id);
		onSignal(msg);
		onIPCSignal(msg);
	}

	if(m_checker)
	{
		m_checker->Stop();
		m_checker = 0;
	}
	
	m_internalConnections.CheckObjects(Ref(this, &IPCConnector::InternalDestroyNotify));	
	m_manager->Stop();
	m_manager = 0;
}

void IPCConnector::SubscribeConnector(const IPCConnector* connector)
{
	IPCConnector* conn = const_cast<IPCConnector*>(connector);
	if(conn)
	{
		ipcSubscribe(conn, SIGNAL_FUNC(this, IPCConnector, IPCProtoMessage, onIPCMessage));
		ipcSubscribe(conn, SIGNAL_FUNC(this, IPCConnector, ModuleNameMessage, onModuleNameMessage));
		ipcSubscribe(conn, SIGNAL_FUNC(this, IPCConnector, UpdateIPCObjectMessage, onUpdateIPCObjectMessage));
		ipcSubscribe(conn, SIGNAL_FUNC(this, IPCConnector, ModuleStateMessage, onModuleStateMessage));
		ipcSubscribe(conn, SIGNAL_FUNC(this, IPCConnector, RemoveIPCObjectMessage, onRemoveIPCObjectMessage));
	}
}

void IPCConnector::SubscribeModule(::SignalOwner* owner)
{
	owner->addSubscriber(this, SIGNAL_FUNC(this, IPCConnector, ChangeIPCNameMessage, onChangeIPCNameMessage));
	owner->addSubscriber(this, SIGNAL_FUNC(this, IPCConnector, IPCMessageSignal, onIPCMessage));
	owner->addSubscriber(this, SIGNAL_FUNC(this, IPCConnector, InitInternalConnectionMessage, onInitInternalConnectionMessage));
}

IPCObjectName IPCConnector::GetModuleName() const
{
	return m_moduleName;
}

void IPCConnector::SetAccessId(const std::string& accessId)
{
	m_accessId = accessId;
}

std::string IPCConnector::GetAccessId()
{
	return m_accessId;
}
	
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
}

bool IPCConnector::SendData(char* data, int len)
{
	std::string senddata(len + sizeof(int), 0);
	memcpy((char*)senddata.c_str() + sizeof(int), data, len);
	memcpy((char*)senddata.c_str(), &len, sizeof(int));
	return	Connector::SendData((char*)senddata.c_str(), len + sizeof(int));
}

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

void IPCConnector::onModuleNameMessage(const ModuleNameMessage& msg)
{
	if(m_moduleName == msg.ipc_name())
	{
		return;
	}

	AddIPCObjectMessage aoMsg(&m_handler);
	aoMsg.set_ip(msg.ip());
	aoMsg.set_port(msg.port());
	aoMsg.set_access_id(msg.access_id());
	*aoMsg.mutable_ipc_name() = msg.ipc_name();
	toMessage(aoMsg);
}

void IPCConnector::onModuleStateMessage(const ModuleStateMessage& msg)
{
	if(msg.id() == m_id)
	{
		const_cast<ModuleStateMessage&>(msg).set_exist(true);
		if(m_rand < msg.rndval())
		{
			const_cast<ModuleStateMessage&>(msg).set_rndval(m_rand);
		}
	}
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

void IPCConnector::addIPCSubscriber(SignalReceiver* receiver, IReceiverFunc* func)
{
	receiver->addSignal(m_ipcSignal, func);
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
	LOG_INFO("Set module name: old-%s, new-%s\n", m_moduleName.GetModuleNameString().c_str(), const_cast<IPCObjectName&>(moduleName).GetModuleNameString().c_str());
	m_moduleName = moduleName;
	return true;
}

void IPCConnector::OnConnected()
{
 	m_bConnected = true;
	ConnectedMessage msg(GetId(), !m_isCoordinator);
	onSignal(msg);
}

void IPCConnector::OnDisconnected()
{
	m_bConnected = false;
}

void IPCConnector::OnAddIPCObject(const std::string& moduleName)
{
}

void IPCConnector::OnUpdateIPCObject(const std::string& oldModuleName, const std::string& newModuleName)
{
}

IPCObjectName IPCConnector::GetIPCName()
{
	return IPCObjectName::GetIPCName(GetId());
}


void IPCConnector::onDisconnected(const DisconnectedMessage& msg)
{
	InternalConnectionStatusMessage icsMsg(&m_handler);
	IPCName* name = icsMsg.mutable_target();
	*name = GetModuleName();
	name->set_conn_id(msg.m_id);
	icsMsg.set_status(CONN_CLOSE);
	toMessage(icsMsg);
	m_internalConnections.DestroyObject(msg.m_id, Ref(this, &IPCConnector::InternalDestroyNotify));
}

void IPCConnector::onCreatedListener(const CreatedListenerMessage& msg)
{
	InternalConnection conn(msg.m_id);
	m_internalConnections.GetObject(conn, &conn);
	conn.m_status = CONN_OPEN;
	m_internalConnections.UpdateObject(conn);

	InternalConnectionStatusMessage icsMsg(&m_handler);
	icsMsg.set_status(CONN_OPEN);
	icsMsg.set_port(msg.m_port);
	IPCName* name = icsMsg.mutable_target();
	*name = IPCObjectName::GetIPCName(GetId());
	name->set_conn_id(msg.m_id);
	onSignal(icsMsg);
}

void IPCConnector::onErrorListener(const ListenErrorMessage& msg)
{
	InternalConnectionStatusMessage icsMsg(&m_handler);
	IPCName* name = icsMsg.mutable_target();
	*name = GetModuleName();
	name->set_conn_id(msg.m_id);
	icsMsg.set_status(CONN_CLOSE);
	toMessage(icsMsg);
	*name = IPCObjectName::GetIPCName(GetId());
	name->set_conn_id(msg.m_id);
	icsMsg.set_status(CONN_FAILED);
	onSignal(icsMsg);
	InternalConnection conn(msg.m_id);
	m_internalConnections.DestroyObject(conn, Ref(this, &IPCConnector::InternalDestroyNotify));
}

void IPCConnector::onErrorConnect(const ConnectErrorMessage& msg)
{
	InternalConnectionStatusMessage icsMsg(&m_handler);
	IPCName* name = icsMsg.mutable_target();
	*name = GetModuleName();
	name->set_conn_id(msg.m_moduleName);
	icsMsg.set_status(CONN_FAILED);
	toMessage(icsMsg);
	InternalConnection conn(msg.m_moduleName);
	m_internalConnections.DestroyObject(conn, Ref(this, &IPCConnector::InternalDestroyNotify));
}
	
void IPCConnector::onAddConnector(const ConnectorMessage& msg)
{
	InternalConnection conn(msg.m_conn->GetId());
	m_internalConnections.GetObject(conn, &conn);
	if(conn.m_thread)
	{
		conn.m_thread->Stop();
		ThreadManager::GetInstance().AddThread(conn.m_thread);
		conn.m_thread = 0;
	}
	else
	{
		InternalConnectionStatusMessage icsMsg(&m_handler);
		IPCName *name = icsMsg.mutable_target();
		*name = IPCObjectName::GetIPCName(GetId());
		name->set_conn_id(msg.m_conn->GetId());
		icsMsg.set_status(CONN_OPEN);
		onSignal(icsMsg);
		*name = GetModuleName();
		name->set_conn_id(msg.m_conn->GetId());
		toMessage(icsMsg);
		conn.m_status = CONN_OPEN;
	}
	m_internalConnections.UpdateObject(conn);
		
	InternalConnector* connector = dynamic_cast<InternalConnector*>(msg.m_conn);
	if(connector)
	{
			
		connector->addSubscriber(this, SIGNAL_FUNC(this, IPCConnector, InternalConnectionDataSignal, onInternalConnectionDataSignal));
		connector->SubscribeConnector(dynamic_cast<SignalOwner*>(this));
		if(m_manager)
			m_manager->AddConnection(msg.m_conn);
	}
	else
	{
		delete msg.m_conn;
	}
}
	
void IPCConnector::onInitInternalConnectionMessage(const InitInternalConnectionMessage& msg)
{
	IPCObjectName target(msg.target());
	target.clear_conn_id();
	if(target.GetModuleNameString() == GetId())
	{
		InternalConnection conn(msg.target().conn_id());
		if(m_isCoordinator || msg.target().conn_id().empty())
		{
			InternalConnectionStatusMessage icsMsg(&m_handler);
			*icsMsg.mutable_target() = msg.target();
			icsMsg.set_status(CONN_FAILED);
			onSignal(icsMsg);
		}
		else if(m_internalConnections.AddObject(conn))
		{
			const_cast<IPCName&>(msg.target()).set_module_name(m_moduleName.module_name());
			const_cast<IPCName&>(msg.target()).set_host_name(m_moduleName.host_name());
			toMessage(msg);
			return;
		}
		else
		{
			InternalConnectionStatusMessage icsMsg(&m_handler);
			*icsMsg.mutable_target() = msg.target();
			icsMsg.set_status(CONN_EXIST);
			onSignal(icsMsg);
		}
	}
}

void IPCConnector::onInternalConnectionDataSignal(const InternalConnectionDataSignal& msg)
{
	InternalConnectionDataMessage icdMsg(&m_handler, msg);
	toMessage(icdMsg);
}

bool IPCConnector::InternalDestroyNotify(const InternalConnection& conn)
{
	if(conn.m_thread)
	{
		conn.m_thread->Stop();
		ThreadManager::GetInstance().AddThread(conn.m_thread);
	}

	InternalConnectionStatusMessage icsMsg(&m_handler);
	IPCName* name = icsMsg.mutable_target();
	*name = IPCObjectName::GetIPCName(GetId());
	name->set_conn_id(conn.m_id);
	icsMsg.set_status((conn.m_status == CONN_OPEN) ? CONN_CLOSE : CONN_FAILED);
	onSignal(icsMsg);
	return true;
}