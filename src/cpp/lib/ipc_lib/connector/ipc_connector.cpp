#include "ipc_connector.h"
#include "module/ipc_module.h"

#include "common/common_func.h"
#include "utils/utils.h"

#include "connector_lib/message/connector_messages.h"

#pragma warning(disable: 4355)

IPCConnector::IPCConnector(AnySocket* socket, const IPCObjectName& moduleName)
: Connector(socket), m_moduleName(moduleName), m_bConnected(false)
, m_checker(0), m_isExist(false), m_rand(CreateGUID())
{
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

		if(len < 0 || len > 0xffff)
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
	m_isNotifyRemove = m_isCoordinator = msg.m_isCoordinator;

	ProtoMessage<ModuleName> mnMsg(this);
	*mnMsg.mutable_ipc_name() = m_moduleName;
	mnMsg.set_ip(msg.m_ip);
	mnMsg.set_port(msg.m_port);
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

	IPCObjectName ipcName(msg.ipc_name());
	m_id = ipcName.GetModuleNameString();

	AddIPCObjectMessage aoMsg(this);
	aoMsg.set_ip(msg.ip());
	aoMsg.set_port(msg.port());
	*aoMsg.mutable_ipc_name() = msg.ipc_name();
	onSignal(aoMsg);

	ModuleNameMessage mnMsg(this, msg);
	mnMsg.set_is_exist(false);
	mnMsg.set_conn_id(m_connectorId);
	onSignal(mnMsg);

	m_isExist = mnMsg.is_exist();
	ModuleStateMessage msMsg(this);
	msMsg.set_rndval("");
	msMsg.set_exist(m_isExist);
	msMsg.set_rndval(m_rand);
	toMessage(msMsg);

	if(m_isExist)
	{
		return;
	}

	if(m_isCoordinator)
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
			Stop();
		}
		else if(msMsg.exist() && m_rand == msMsg.rndval())
		{
			printf("random values is equal\n");
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
	if(msg.ipc_path_size() == 0 || !m_bConnected)
	{
		IPCMessageSignal signalMsg(msg);
		onSignal(signalMsg);
		return;
	}

	IPCMessage newMsg(msg);
	newMsg.clear_ipc_path();
	IPCObjectName newPath(msg.ipc_path(0));
	if(newPath == m_moduleName)
	{
		for(int i = 1; i < msg.ipc_path_size(); i++)
		{
			*newMsg.add_ipc_path() = msg.ipc_path(i);
		}
	}
	*newMsg.add_ipc_sender() = GetIPCName();

	if(newPath == m_moduleName && !newMsg.ipc_path_size())
	{
		std::string data;
		data.resize(sizeof(int));
		int sizeName = (int)msg.message_name().size();
		memcpy((char*)data.c_str(), &sizeName, sizeof(int));
		data.append(msg.message_name());
		data.append(msg.message().c_str(), msg.message().size());
		if(!onData((char*)data.c_str(), (int)data.size()))
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

	printf("change connector id - %s\n", m_id.c_str());
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

void IPCConnector::onNewConnector(const Connector* connector)
{
	IPCConnector* conn = const_cast<IPCConnector*>(static_cast<const IPCConnector*>(connector));
	if(conn)
	{
		addIPCSubscriber(conn, SIGNAL_FUNC(conn, IPCConnector, IPCProtoMessage, onIPCMessage));
		addIPCSubscriber(conn, SIGNAL_FUNC(conn, IPCConnector, ModuleNameMessage, onModuleNameMessage));
		addIPCSubscriber(conn, SIGNAL_FUNC(conn, IPCConnector, UpdateIPCObjectMessage, onUpdateIPCObjectMessage));
		addIPCSubscriber(conn, SIGNAL_FUNC(conn, IPCConnector, ModuleStateMessage, onModuleStateMessage));
		addIPCSubscriber(conn, SIGNAL_FUNC(conn, IPCConnector, RemoveIPCObjectMessage, onRemoveIPCObjectMessage));
		ipcSubscribe(conn, SIGNAL_FUNC(this, IPCConnector, IPCProtoMessage, onIPCMessage));
		ipcSubscribe(conn, SIGNAL_FUNC(this, IPCConnector, ModuleNameMessage, onModuleNameMessage));
		ipcSubscribe(conn, SIGNAL_FUNC(this, IPCConnector, UpdateIPCObjectMessage, onUpdateIPCObjectMessage));
		ipcSubscribe(conn, SIGNAL_FUNC(this, IPCConnector, ModuleStateMessage, onModuleStateMessage));
		ipcSubscribe(conn, SIGNAL_FUNC(this, IPCConnector, RemoveIPCObjectMessage, onRemoveIPCObjectMessage));
	}
}

void IPCConnector::Subscribe(::SignalOwner* owner)
{
	owner->addSubscriber(this, SIGNAL_FUNC(this, IPCConnector, ChangeIPCNameMessage, onChangeIPCNameMessage));
	owner->addSubscriber(this, SIGNAL_FUNC(this, IPCConnector, IPCMessageSignal, onIPCMessage));
}

IPCObjectName IPCConnector::GetModuleName() const
{
	return m_moduleName;
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