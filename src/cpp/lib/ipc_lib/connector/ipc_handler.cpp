#include "ipc_handler.h"
#include "ipc_connector.h"
#include "internal_connector.h"
#include "common/logger.h"
#include "common/ref.h"
#include "module/ipc_module.h"
#include "connector_lib/socket/socket_factories.h"
#include "connector_lib/thread/connect_thread.h"
#include "thread_lib/thread/thread_manager.h"

IPCHandler::IPCHandler(IPCConnector* connector)
: m_connector(connector)
{
}

IPCHandler::~IPCHandler()
{
}
	
void IPCHandler::onMessage(const ModuleName& msg)
{
	if(m_connector->m_checker)
	{
		m_connector->m_checker->Stop();
		m_connector->m_checker = 0;
	}
	
	bool isCoordinatorAccess = m_connector->m_isCoordinator || m_connector->m_id == IPCModule::m_coordinatorIPCName;
	isCoordinatorAccess ? m_connector->SetAccessId(IPCModule::m_baseAccessId) : void(0);	
	if(!isCoordinatorAccess && m_connector->m_accessId != msg.access_id())
	{
		LOG_INFO("access denied: m_id-%s, m_module-%s, module_access-%s, id_access-%s\n",
			 m_connector->m_id.c_str(), m_connector->m_moduleName.GetModuleNameString().c_str(),
			 m_connector->m_accessId.c_str(), msg.access_id().c_str());
		m_connector->Stop();
	}
	
	IPCObjectName ipcName(msg.ipc_name());
	m_connector->m_id = ipcName.GetModuleNameString();

	LOG_INFO("ModuleName message: m_id-%s, m_module-%s\n",
		 m_connector->m_id.c_str(), m_connector->m_moduleName.GetModuleNameString().c_str());

	AddIPCObjectMessage aoMsg(this);
	aoMsg.set_ip(msg.ip());
	aoMsg.set_port(msg.port());
	m_connector->m_isCoordinator ? aoMsg.set_access_id(msg.access_id()) : aoMsg.set_access_id(m_connector->m_accessId);
	*aoMsg.mutable_ipc_name() = msg.ipc_name();
	m_connector->onSignal(aoMsg);

	ModuleNameMessage mnMsg(this, msg);
	mnMsg.set_is_exist(false);
	mnMsg.set_conn_id(m_connector->m_connectorId);
	m_connector->m_isCoordinator ? mnMsg.set_access_id(msg.access_id()) : mnMsg.set_access_id(m_connector->m_accessId);
	m_connector->onSignal(mnMsg);

	m_connector->m_isExist = mnMsg.is_exist();
	ModuleStateMessage msMsg(this);
	msMsg.set_rndval("");
	msMsg.set_exist(m_connector->m_isExist);
	msMsg.set_rndval(m_connector->m_rand);
	m_connector->toMessage(msMsg);

	if(m_connector->m_isSendIPCObjects)
	{
		IPCObjectListMessage ipcolMsg(this);
		ipcolMsg.set_access_id(msg.access_id());
		m_connector->onSignal(ipcolMsg);
		m_connector->toMessage(ipcolMsg);
	}
	
	if(m_connector->m_isExist)
	{
		LOG_INFO("Module exists: m_id-%s, m_module-%s\n", m_connector->m_id.c_str(), m_connector->m_moduleName.GetModuleNameString().c_str());
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

void IPCHandler::onMessage(const ModuleState& msg)
{
	if(msg.exist() && m_connector->m_isExist)
	{
		LOG_INFO("Module exists: m_id-%s, m_module-%s\n",
			 m_connector->m_id.c_str(), m_connector->m_moduleName.GetModuleNameString().c_str());
		m_connector->Stop();
	}
	else if(msg.exist() && !m_connector->m_isExist)
	{
		ModuleStateMessage msMsg(this, msg);
		msMsg.set_exist(false);
		msMsg.set_id(m_connector->m_id);
		m_connector->onIPCSignal(msMsg);

		if (msMsg.exist() && m_connector->m_rand > msMsg.rndval()
			|| !msMsg.exist())
		{
			LOG_INFO("Module exists: m_id-%s, m_module-%s\n",
				 m_connector->m_id.c_str(), m_connector->m_moduleName.GetModuleNameString().c_str());
			m_connector->Stop();
		}
		else if(msMsg.exist() && m_connector->m_rand == msMsg.rndval())
		{
			LOG_WARNING("Random values is equal: m_id-%s, m_module-%s\n",
				    m_connector->m_id.c_str(), m_connector->m_moduleName.GetModuleNameString().c_str());
		}
		else
		{
			m_connector->OnConnected();
		}
	}
}

void IPCHandler::onMessage(const AddIPCObject& msg)
{
	AddIPCObjectMessage aoMsg(this, msg);
	m_connector->onSignal(aoMsg);

	IPCObjectName ipcName(msg.ipc_name());
	m_connector->OnAddIPCObject(ipcName.GetModuleNameString());
}

void IPCHandler::onMessage(const RemoveIPCObject& msg)
{
	RemoveIPCObjectMessage roMsg(this, msg);
	m_connector->onSignal(roMsg);
}

void IPCHandler::onMessage(const IPCMessage& msg)
{
	if(!m_connector->m_bConnected)
	{
		return;
	}
	
	bool isTarget = (msg.ipc_path_size() == 0);

	IPCMessage newMsg(msg);
	newMsg.clear_ipc_path();
	if(!isTarget)
	{
		IPCObjectName newPath(msg.ipc_path(0));
		if(newPath == m_connector->m_moduleName)
		{
			for(int i = 1; i < msg.ipc_path_size(); i++)
			{
				*newMsg.add_ipc_path() = msg.ipc_path(i);
			}
		}
		*newMsg.add_ipc_sender() = m_connector->GetIPCName();
		isTarget = (newPath == m_connector->m_moduleName && !newMsg.ipc_path_size());
	}

	if (isTarget &&
	    !m_connector->onData(msg.message_name(), (char*)msg.message().c_str(), (int)msg.message().size()))
	{
		IPCProtoMessage protoMsg(this, newMsg);
		m_connector->onSignal(protoMsg);
	}
	else if(newMsg.ipc_path_size())
	{
		IPCProtoMessage protoMsg(this, newMsg);
		m_connector->onIPCSignal(protoMsg);
	}
}

void IPCHandler::onMessage(const IPCObjectList& msg)
{
	for(int i = 0; i < msg.ipc_object_size(); i++)
	{
		AddIPCObjectMessage aoMsg(this, msg.ipc_object(i));
		m_connector->onSignal(aoMsg);
		
		IPCObjectName ipcName(msg.ipc_object(i).ipc_name());
		m_connector->OnAddIPCObject(ipcName.GetModuleNameString());
	}
}

void IPCHandler::onMessage(const ChangeIPCName& msg)
{
	UpdateIPCObjectMessage uioMsg(this);
	*uioMsg.mutable_ipc_new_name() = msg.ipc_name();
	*uioMsg.mutable_ipc_old_name() = IPCObjectName::GetIPCName(m_connector->m_id);
	m_connector->onSignal(uioMsg);

	if(m_connector->m_isCoordinator)
	{
		m_connector->onIPCSignal(uioMsg);
	}

	IPCObjectName ipcName(msg.ipc_name());
	m_connector->m_id = ipcName.GetModuleNameString();
	
	LOG_INFO("Change connector id: m_id-%s, m_module-%s\n", m_connector->m_id.c_str(), m_connector->m_moduleName.GetModuleNameString().c_str());
}

void IPCHandler::onMessage(const UpdateIPCObject& msg)
{
	UpdateIPCObjectMessage uioMsg(this, msg);
	m_connector->onSignal(uioMsg);

	IPCObjectName ipcNameOld(msg.ipc_old_name());
	IPCObjectName ipcNameNew(msg.ipc_new_name());
	m_connector->OnUpdateIPCObject(ipcNameOld.GetModuleNameString(), ipcNameNew.GetModuleNameString());
}

void IPCHandler::onMessage(const Ping& msg)
{
}

void IPCHandler::onMessage(const InitInternalConnection& msg)
{
	if(m_connector->m_isCoordinator)
	{
		InternalConnectionStatusMessage icsMsg(this);
		IPCName* name = icsMsg.mutable_target();
		*name = m_connector->GetModuleName();
		name->set_conn_id(msg.target().conn_id());
		icsMsg.set_status(CONN_FAILED);
		m_connector->toMessage(icsMsg);
	}
	else
	{
		IPCConnector::InternalConnection conn(msg.target().conn_id());
		if(m_connector->m_internalConnections.AddObject(conn))
		{
			ConnectAddress address;
			address.m_localIP = "";
			address.m_localPort = 0;
			address.m_moduleName = address.m_id = msg.target().conn_id();
			address.m_connectorFactory = new SimpleConnectorFactory<InternalConnector>;
			address.m_socketFactory = new TCPSocketFactory(m_connector->m_socket->m_ipv);
			address.m_ip = msg.ip();
			address.m_port = msg.port();
			ConnectThread* thread = new ConnectThread(address);
			thread->addSubscriber(m_connector, SIGNAL_FUNC(m_connector, IPCConnector, ConnectorMessage, onAddConnector));
			thread->addSubscriber(m_connector, SIGNAL_FUNC(m_connector, IPCConnector, ConnectErrorMessage, onErrorConnect));
			thread->Start();
		}
		else
		{
			InternalConnectionStatusMessage icsMsg(this);
			IPCName* name = icsMsg.mutable_target();
			*name = m_connector->GetModuleName();
			name->set_conn_id(msg.target().conn_id());
			icsMsg.set_status(CONN_EXIST);
			m_connector->toMessage(icsMsg);
		}
	}
}

void IPCHandler::onMessage(const InternalConnectionStatus& msg)
{
	IPCConnector::InternalConnection conn(msg.target().conn_id());
	m_connector->m_internalConnections.GetObject(conn, &conn);
	switch(msg.status())
	{
		case CONN_OPEN:
		{
			ListenAddress address;
			address.m_id = msg.target().conn_id();
			address.m_localIP = "127.0.0.1";
			address.m_localPort = 0;
			address.m_connectorFactory = new SimpleConnectorFactory<InternalConnector>;
			address.m_socketFactory = new TCPSocketFactory(m_connector->m_socket->m_ipv);
			address.m_acceptCount = 1;
			BaseListenThread *listenThread = new BaseListenThread(address);
			listenThread->addSubscriber(m_connector, SIGNAL_FUNC(m_connector, IPCConnector, CreatedListenerMessage, onCreatedListener));
			listenThread->addSubscriber(m_connector, SIGNAL_FUNC(m_connector, IPCConnector, ListenErrorMessage, onErrorListener));
			listenThread->addSubscriber(m_connector, SIGNAL_FUNC(m_connector, IPCConnector, ConnectorMessage, onAddConnector));
			conn.m_thread = listenThread;
			m_connector->m_internalConnections.UpdateObject(conn);
			listenThread->Start();
			return;
		}
		case CONN_CLOSE:
		{
			m_connector->m_internalConnections.DestroyObject(conn, Ref(m_connector, &IPCConnector::InternalDestroyNotify));
			m_connector->m_manager->StopConnection(msg.target().conn_id());
			return;
		}
		case CONN_EXIST:
		case CONN_FAILED:
		{
			m_connector->m_internalConnections.DestroyObject(conn, Ref(m_connector, &IPCConnector::InternalDestroyNotify));
			break;
		}
	}

	InternalConnectionStatusMessage icsMsg(this, msg);
	*icsMsg.mutable_target() = IPCObjectName::GetIPCName(m_connector->GetId());
	const_cast<IPCName&>(icsMsg.target()).set_conn_id(msg.target().conn_id());
	m_connector->onSignal(icsMsg);
}

void IPCHandler::onMessage(const InternalConnectionData& msg)
{
	InternalConnectionDataMessage icdMsg(this, msg);
	m_connector->onSignal(icdMsg);
}