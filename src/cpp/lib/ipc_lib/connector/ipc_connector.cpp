#include <stdio.h>

#include "ipc_connector.h"
#include "internal_connector.h"
#include "module/ipc_module.h"

#include "common/common_func.h"
#include "common/logger.h"
#include "common/user.h"
#include "utils/utils.h"

#include "connector_lib/message/connector_messages.h"
#include "connector_lib/socket/socket_factories.h"
#include "connector_lib/thread/connect_thread.h"
#include "thread_lib/thread/thread_manager.h"

#pragma warning(disable: 4355)

#define MAX_DATA_LEN		1024*1024

IPCConnector::IPCConnector(AnySocket* socket, const IPCObjectName& moduleName)
: Connector(socket), m_moduleName(moduleName)
, m_bConnected(false), m_isCoordinator(false)
, m_isNotifyRemove(true), m_isSendIPCObjects(false)
, m_checker(0), m_isExist(false), m_rand(CreateGUID())
{
	m_manager = new ConnectorManager;
	m_manager->addSubscriber(this, SIGNAL_FUNC(this, IPCConnector, DisconnectedMessage, onDisconnected));
	m_ipcSignal = new Signal(static_cast<SignalOwner*>(this));
	addMessage(new ProtoMessage<ModuleName>(this));
	addMessage(new ProtoMessage<AddIPCObject>(this));
	addMessage(new ProtoMessage<RemoveIPCObject>(this));
	addMessage(new ProtoMessage<IPCMessage>(this));
	addMessage(new ProtoMessage<IPCObjectList>(this));
	addMessage(new ProtoMessage<ChangeIPCName>(this));
	addMessage(new ProtoMessage<UpdateIPCObject>(this));
	addMessage(new ProtoMessage<ModuleState>(this));
	addMessage(new ProtoMessage<Ping>(this));
		
	addMessage(new ProtoMessage<InitInternalConnection>(this));
	addMessage(new ProtoMessage<InternalConnectionStatus>(this));
	addMessage(new ProtoMessage<InternalConnectionData>(this));
}

IPCConnector::~IPCConnector()
{
	m_ipcSignal->removeOwner();
	removeReceiver();

	CSLocker locker(&m_cs);
	for(std::map<std::string, ListenThread*>::iterator it = m_internalListener.begin();
		it != m_internalListener.end(); it++)
	{
		it->second->Stop();
		ThreadManager::GetInstance().AddThread(it->second);
	}
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
	m_accessId = GetUserName();

	ProtoMessage<ModuleName> mnMsg(this);
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
		RemoveIPCObjectMessage msg(this);
		msg.set_ipc_name(m_id);
		onSignal(msg);
		onIPCSignal(msg);
	}

	if(m_checker)
	{
		m_checker->Stop();
		m_checker = 0;
	}
}

void IPCConnector::onMessage(const ModuleName& msg)
{
	if(m_checker)
	{
		m_checker->Stop();
		m_checker = 0;
	}
	
	bool isCoordinatorAccess = m_isCoordinator || m_id == IPCModule::m_coordinatorIPCName;
	m_isCoordinator ? m_accessId = IPCModule::m_baseAccessId : m_accessId;	
	if(!isCoordinatorAccess && m_accessId != msg.access_id())
	{
		LOG_INFO("access denied: m_id-%s, m_module-%s, module_access-%s, id_access-%s\n",
			 m_id.c_str(), m_moduleName.GetModuleNameString().c_str(), m_accessId.c_str(), msg.access_id().c_str());
		Stop();
	}
	
	IPCObjectName ipcName(msg.ipc_name());
	m_id = ipcName.GetModuleNameString();

	LOG_INFO("ModuleName message: m_id-%s, m_module-%s\n", m_id.c_str(), m_moduleName.GetModuleNameString().c_str());

	AddIPCObjectMessage aoMsg(this);
	aoMsg.set_ip(msg.ip());
	aoMsg.set_port(msg.port());
	m_isCoordinator ? aoMsg.set_access_id(msg.access_id()) : aoMsg.set_access_id(m_accessId);
	*aoMsg.mutable_ipc_name() = msg.ipc_name();
	onSignal(aoMsg);

	ModuleNameMessage mnMsg(this, msg);
	mnMsg.set_is_exist(false);
	mnMsg.set_conn_id(m_connectorId);
	m_isCoordinator ? mnMsg.set_access_id(msg.access_id()) : mnMsg.set_access_id(m_accessId);
	onSignal(mnMsg);

	m_isExist = mnMsg.is_exist();
	ModuleStateMessage msMsg(this);
	msMsg.set_rndval("");
	msMsg.set_exist(m_isExist);
	msMsg.set_rndval(m_rand);
	toMessage(msMsg);

	if(m_isExist)
	{
		LOG_INFO("Module exists: m_id-%s, m_module-%s\n", m_id.c_str(), m_moduleName.GetModuleNameString().c_str());
		return;
	}

	if(m_isSendIPCObjects)
	{
		IPCObjectListMessage ipcolMsg(this);
		onSignal(ipcolMsg);
		toMessage(ipcolMsg);

		onIPCSignal(mnMsg);
	}
	OnConnected();
}

void IPCConnector::onMessage(const ModuleState& msg)
{
	if(msg.exist() && m_isExist)
	{
		LOG_INFO("Module exists: m_id-%s, m_module-%s\n", m_id.c_str(), m_moduleName.GetModuleNameString().c_str());
		Stop();
	}
	else if(msg.exist() && !m_isExist)
	{
		ModuleStateMessage msMsg(this, msg);
		msMsg.set_exist(false);
		msMsg.set_id(m_id);
		onIPCSignal(msMsg);

		if (msMsg.exist() && m_rand > msMsg.rndval()
			|| !msMsg.exist())
		{
			LOG_INFO("Module exists: m_id-%s, m_module-%s\n", m_id.c_str(), m_moduleName.GetModuleNameString().c_str());
			Stop();
		}
		else if(msMsg.exist() && m_rand == msMsg.rndval())
		{
			LOG_WARNING("Random values is equal: m_id-%s, m_module-%s\n", m_id.c_str(), m_moduleName.GetModuleNameString().c_str());
		}
		else
		{
			OnConnected();
		}
	}
}

void IPCConnector::onMessage(const AddIPCObject& msg)
{
	AddIPCObjectMessage aoMsg(this, msg);
	onSignal(aoMsg);

	IPCObjectName ipcName(msg.ipc_name());
	OnAddIPCObject(ipcName.GetModuleNameString());
}

void IPCConnector::onMessage(const RemoveIPCObject& msg)
{
	RemoveIPCObjectMessage roMsg(this, msg);
	onSignal(roMsg);
}

void IPCConnector::onMessage(const IPCMessage& msg)
{
	if(!m_bConnected)
	{
		return;
	}
	
	bool isTarget = (msg.ipc_path_size() == 0);

	IPCMessage newMsg(msg);
	newMsg.clear_ipc_path();
	if(!isTarget)
	{
		IPCObjectName newPath(msg.ipc_path(0));
		if(newPath == m_moduleName)
		{
			for(int i = 1; i < msg.ipc_path_size(); i++)
			{
				*newMsg.add_ipc_path() = msg.ipc_path(i);
			}
		}
		*newMsg.add_ipc_sender() = GetIPCName();
		isTarget = (newPath == m_moduleName && !newMsg.ipc_path_size());
	}

	if(isTarget && (newMsg.access_id() == m_accessId || m_isCoordinator))
	{
		std::string data;
		data.resize(sizeof(int));
		int sizeName = (int)msg.message_name().size();
		memcpy((char*)data.c_str(), &sizeName, sizeof(int));
		data.append(msg.message_name());
		data.append(msg.message().c_str(), msg.message().size());
		if(!onData((char*)data.c_str(), (int)data.size()) && !m_isCoordinator)
		{
			IPCProtoMessage protoMsg(this, newMsg);
			onSignal(protoMsg);
		}
	}
	else if(newMsg.ipc_path_size())
	{
		IPCProtoMessage protoMsg(this, newMsg);
		onIPCSignal(protoMsg);
	}
}

void IPCConnector::onMessage(const IPCObjectList& msg)
{
	for(int i = 0; i < msg.ipc_object_size(); i++)
	{
		AddIPCObjectMessage aoMsg(this, msg.ipc_object(i));
		onSignal(aoMsg);
		
		IPCObjectName ipcName(msg.ipc_object(i).ipc_name());
		OnAddIPCObject(ipcName.GetModuleNameString());
	}
}

void IPCConnector::onMessage(const ChangeIPCName& msg)
{
	UpdateIPCObjectMessage uioMsg(this);
	*uioMsg.mutable_ipc_new_name() = msg.ipc_name();
	*uioMsg.mutable_ipc_old_name() = IPCObjectName::GetIPCName(m_id);
	onSignal(uioMsg);

	if(m_isCoordinator)
	{
		onIPCSignal(uioMsg);
	}

	IPCObjectName ipcName(msg.ipc_name());
	m_id = ipcName.GetModuleNameString();
	
	LOG_INFO("Change connector id: m_id-%s, m_module-%s\n", m_id.c_str(), m_moduleName.GetModuleNameString().c_str());
}

void IPCConnector::onMessage(const UpdateIPCObject& msg)
{
	UpdateIPCObjectMessage uioMsg(this, msg);
	onSignal(uioMsg);

	IPCObjectName ipcNameOld(msg.ipc_old_name());
	IPCObjectName ipcNameNew(msg.ipc_new_name());
	OnUpdateIPCObject(ipcNameOld.GetModuleNameString(), ipcNameNew.GetModuleNameString());
}

void IPCConnector::onMessage(const Ping& msg)
{
}

void IPCConnector::onMessage(const InitInternalConnection& msg)
{
	if(m_isCoordinator)
	{
		InternalConnectionStatusMessage icsMsg(this);
		icsMsg.set_id(msg.id());
		icsMsg.set_status(CONN_FAILED);
		toMessage(icsMsg);
	}
	else
	{	
		ConnectAddress address;
		address.m_localIP = "";
		address.m_localPort = 0;
		address.m_moduleName = address.m_id = msg.id();
		address.m_connectorFactory = new SimpleConnectorFactory<InternalConnector>;
		address.m_socketFactory = new TCPSocketFactory;
		address.m_ip = msg.ip();
		address.m_port = msg.port();
		ConnectThread* thread = new ConnectThread(address);
		thread->addSubscriber(this, SIGNAL_FUNC(this, IPCConnector, ConnectorMessage, onAddConnector));
		thread->addSubscriber(this, SIGNAL_FUNC(this, IPCConnector, ConnectErrorMessage, onErrorConnect));
		thread->Start();
	}
}

void IPCConnector::onMessage(const InternalConnectionStatus& msg)
{
	switch(msg.status())
	{
		case CONN_OPEN:
		{
			ListenAddress address;
			address.m_id = msg.id();
			address.m_localIP = "127.0.0.1";
			address.m_localPort = 0;
			address.m_connectorFactory = new SimpleConnectorFactory<InternalConnector>;
			address.m_socketFactory = new TCPSocketFactory;
			address.m_acceptCount = 1;
			BaseListenThread *listenThread = new BaseListenThread(address);
			listenThread->addSubscriber(this, SIGNAL_FUNC(this, IPCConnector, CreatedListenerMessage, onCreatedListener));
			listenThread->addSubscriber(this, SIGNAL_FUNC(this, IPCConnector, ListenErrorMessage, onErrorListener));
			listenThread->addSubscriber(this, SIGNAL_FUNC(this, IPCConnector, ConnectorMessage, onAddConnector));
			listenThread->Start();
			CSLocker locker(&m_cs);
			m_internalListener.insert(std::make_pair(msg.id(), listenThread));
			break;
		}
		case CONN_CLOSE:
		{
			m_manager->StopConnection(msg.id());
			CSLocker locker(&m_cs);
			std::map<std::string, ListenThread*>::iterator itListen = m_internalListener.find(msg.id());
			if(itListen != m_internalListener.end())
			{
				itListen->second->Stop();
				ThreadManager::GetInstance().AddThread(itListen->second);
				m_internalListener.erase(itListen);
			}
		}
		case CONN_FAILED:
		{
			InternalConnectionStatusMessage icsMsg(this, msg);
			*icsMsg.mutable_target() = IPCObjectName::GetIPCName(GetId());
			onSignal(icsMsg);
			break;
		}
	}
}

void IPCConnector::onMessage(const InternalConnectionData& msg)
{
	InternalConnectionDataMessage icdMsg(this, msg);
	onSignal(icdMsg);
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
		IPCProtoMessage ipcMsg(this, static_cast<const IPCMessage&>(msg));
		ipcMsg.set_access_id(m_accessId);
		toMessage(ipcMsg);
	}
}

void IPCConnector::onModuleNameMessage(const ModuleNameMessage& msg)
{
	if(m_moduleName == msg.ipc_name())
	{
		return;
	}

	AddIPCObjectMessage aoMsg(this);
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
	InternalConnectionStatusMessage icsMsg(this);
	icsMsg.set_id(msg.m_id);
	icsMsg.set_status(CONN_CLOSE);
	toMessage(icsMsg);
	*icsMsg.mutable_target() = IPCObjectName::GetIPCName(GetId());
	onSignal(icsMsg);
}

void IPCConnector::onCreatedListener(const CreatedListenerMessage& msg)
{
	InternalConnectionStatusMessage icsMsg(this);
	icsMsg.set_id(msg.m_id);
	icsMsg.set_status(CONN_OPEN);
	icsMsg.set_port(msg.m_port);
	*icsMsg.mutable_target() = IPCObjectName::GetIPCName(GetId());
	onSignal(icsMsg);
}

void IPCConnector::onErrorListener(const ListenErrorMessage& msg)
{
	InternalConnectionStatusMessage icsMsg(this);
	icsMsg.set_id(msg.m_id);
	icsMsg.set_status(CONN_CLOSE);
	toMessage(icsMsg);
	icsMsg.set_status(CONN_FAILED);
	*icsMsg.mutable_target() = IPCObjectName::GetIPCName(GetId());
	onSignal(icsMsg);
}

void IPCConnector::onErrorConnect(const ConnectErrorMessage& msg)
{
	InternalConnectionStatusMessage icsMsg(this);
	icsMsg.set_id(msg.m_moduleName);
	icsMsg.set_status(CONN_FAILED);
	toMessage(icsMsg);
}
	
void IPCConnector::onAddConnector(const ConnectorMessage& msg)
{
	InternalConnectionStatusMessage icsMsg(this);
	icsMsg.set_id(msg.m_conn->GetId());
	icsMsg.set_status(CONN_OPEN);
	
	{
		CSLocker locker(&m_cs);
		std::map<std::string, ListenThread*>::iterator itListen = m_internalListener.find(msg.m_conn->GetId());
		if(itListen != m_internalListener.end())
		{
			itListen->second->Stop();
			ThreadManager::GetInstance().AddThread(itListen->second);
			m_internalListener.erase(itListen);
		}
		else
		{
			toMessage(icsMsg);
		}
	}
	
	*icsMsg.mutable_target() = IPCObjectName::GetIPCName(GetId());
	onSignal(icsMsg);
		
	InternalConnector* connector = dynamic_cast<InternalConnector*>(msg.m_conn);
	if(connector)
	{
			
		connector->addSubscriber(this, SIGNAL_FUNC(this, IPCConnector, InternalConnectionDataSignal, onInternalConnectionDataSignal));
		connector->SubscribeConnector(dynamic_cast<SignalOwner*>(this));
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
	if(target.GetModuleNameString() == GetId())
	{
		if(m_isCoordinator)
		{
			InternalConnectionStatusMessage icsMsg(this);
			icsMsg.set_id(msg.id());
			icsMsg.set_status(CONN_FAILED);
			*icsMsg.mutable_target() = IPCObjectName::GetIPCName(GetId());
			onSignal(icsMsg);
		}
		else
		{
			toMessage(msg);
		}
	}
}

void IPCConnector::onInternalConnectionDataSignal(const InternalConnectionDataSignal& msg)
{
	InternalConnectionDataMessage icdMsg(this, msg);
	toMessage(icdMsg);
}