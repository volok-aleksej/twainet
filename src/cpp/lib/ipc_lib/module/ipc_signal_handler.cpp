#include <stdio.h>
#include <time.h>
#include "ipc_signal_handler.h"
#include "ipc_module.h"
#include "thread_lib/thread/thread_manager.h"
#include "common/common.h"
#include "common/logger.h"
#include "common/user.h"

IPCSignalHandler::IPCSignalHandler(IPCModule* module)
: m_module(module)
{
}

IPCSignalHandler::~IPCSignalHandler()
{
	removeReceiver();
}

void IPCSignalHandler::onCreatedListener(const CreatedListenerMessage& msg)
{
	IPCModule::IPCObject object(IPCObjectName::GetIPCName(msg.m_id), msg.m_ip,
				    msg.m_port, m_module->m_isCoordinator ? IPCModule::m_baseAccessId : GetUserName());
	m_module->m_ipcObject.AddObject(object);
	if(!m_module->m_isCoordinator)
	{
		m_module->ConnectToCoordinator();
	}

	LOG_INFO("Module created: m_moduleName - %s\n", m_module->m_moduleName.GetModuleNameString().c_str());
}

void IPCSignalHandler::onErrorListener(const ListenErrorMessage& msg)
{
	if(m_module->m_isCoordinator && m_module->m_countListener + 1 < g_ipcCoordinatorPortCount)
	{
		m_module->m_countListener++;
		m_module->StartAsCoordinator();
	}
	else
	{
		ThreadManager::GetInstance().AddThread(m_module->m_listenThread);
		m_module->m_listenThread = 0;
		m_module->m_isExit = true;
		m_module->ModuleCreationFialed();
	}
}

void IPCSignalHandler::onAddConnector(const ConnectorMessage& msg)
{
	m_module->AddConnector(msg.m_conn);
}

void IPCSignalHandler::onErrorConnect(const ConnectErrorMessage& msg)
{
	if(msg.m_moduleName == m_module->m_coordinatorIPCName && !m_module->m_isExit)
	{
		if(m_module->m_countConnect + 1 == g_ipcCoordinatorPortCount)
		{
			m_module->m_countConnect = 0;
		}
		else
		{
			m_module->m_countConnect++;
		}

		CSLocker locker(&m_module->m_csRequest);
		m_module->m_bConnectToCoordinatorRequest = true;
		time(&m_module->m_requestCreated);
	}
	else
	{
		IPCModule::TryConnectCounter counter(msg.m_moduleName);
		if(!m_module->m_tryConnectCounters.GetObject(counter, &counter))
		{
			m_module->OnConnectFailed(msg.m_moduleName);
		}

		if(counter.m_count < m_module->m_maxTryConnectCount && !m_module->m_isExit)
		{
			m_module->ConnectTo(IPCObjectName::GetIPCName(msg.m_moduleName));
			counter.m_count++;
			m_module->m_tryConnectCounters.UpdateObject(counter);
		}
		else
		{
			m_module->OnConnectFailed(msg.m_moduleName);
			m_module->m_tryConnectCounters.RemoveObject(counter);
		}
	}
}

void IPCSignalHandler::onConnected(const ConnectedMessage& msg)
{
	m_module->OnConnected(msg.m_id);
	if(msg.m_bWithCoordinator)
	{
		LOG_INFO("Module connected with coordinator: coordinator name - %s, m_moduleName - %s\n",
			 m_module->m_coordinatorName.c_str(), m_module->m_moduleName.GetModuleNameString().c_str());
		m_module->m_coordinatorName = msg.m_id;
	}
}

void IPCSignalHandler::onInternalConnectionStatusMessage(const InternalConnectionStatusMessage& msg)
{
	IPCObjectName target(msg.target());
	if(msg.status() == interconn::CONN_OPEN)
	{
		IPCModule::IPCObject object(msg.target(), "127.0.0.1", msg.port(), m_module->m_internalAccessId);
		m_module->m_modules.AddObject(object);
		m_module->m_ipcObject.AddObject(object);
	}
	if(msg.status() == interconn::CONN_CLOSE)
	{
		IPCModule::IPCObject object(msg.target(), "127.0.0.1", msg.port(), m_module->m_internalAccessId);
		m_module->m_modules.RemoveObject(object);
		m_module->m_ipcObject.RemoveObject(object);
	}
	m_module->OnInternalConnection(target.GetModuleNameString(), msg.status(), msg.port());
}

void IPCSignalHandler::onIPCMessage(const IPCProtoMessage& msg)
{
	std::vector<std::string> path;
	for(int i = 0; i < msg.ipc_sender_size(); i++)
	{
		IPCObjectName sender(msg.ipc_sender(i));
		path.push_back(sender.GetModuleNameString());
	}
	m_module->OnMessage(msg.message_name(), path, msg.message());
}

void IPCSignalHandler::onAddIPCObject(const AddIPCObjectMessage& msg)
{
	IPCModule::IPCObject object(msg.ipc_name(), msg.ip(), msg.port(), msg.access_id());
	LOG_INFO("Add IPC Object: ipc name - %s, m_moduleName - %s\n",
		 object.m_ipcName.GetModuleNameString().c_str(), m_module->m_moduleName.GetModuleNameString().c_str());
	m_module->m_ipcObject.AddObject(object);
}

void IPCSignalHandler::onUpdateIPCObject(const UpdateIPCObjectMessage& msg)
{
	IPCModule::IPCObject object(msg.ipc_old_name());
	m_module->m_ipcObject.GetObject(object, &object);
	m_module->m_ipcObject.RemoveObject(object);
	bool update = false;
	if(m_module->m_modules.RemoveObject(object))
	{
		update = true;
	}

	m_module->m_csConnectors.Enter();
	std::map<std::string, std::vector<std::string> >::iterator it = m_module->m_connectors.find(object.m_ipcName.GetModuleNameString());
	if(it != m_module->m_connectors.end())
	{
		IPCObjectName newName(msg.ipc_new_name());
		std::vector<std::string> connectors = it->second;
		m_module->m_connectors.erase(it);
		m_module->m_connectors.insert(std::make_pair(newName.GetModuleNameString(), connectors));
	}
	m_module->m_csConnectors.Leave();

	IPCModule::TryConnectCounter counter(object.m_ipcName.GetModuleNameString());
	if(m_module->m_tryConnectCounters.GetObject(counter, &counter))
	{
		m_module->m_tryConnectCounters.RemoveObject(counter);
		IPCObjectName ipcName(msg.ipc_new_name());
		counter.m_moduleName = ipcName.GetModuleNameString();
		m_module->m_tryConnectCounters.AddObject(counter);
	}

	object.m_ipcName = msg.ipc_new_name();

	m_module->m_ipcObject.AddObject(object);
	if(update)
	{
		m_module->m_modules.AddObject(object);
	}

	IPCObjectName ipcNameNew(msg.ipc_new_name());
	IPCObjectName ipcNameOld(msg.ipc_old_name());
	LOG_INFO("Update IPC Object: ipc name old - %s, ipc name new - %s, m_moduleName - %s\n",
			ipcNameOld.GetModuleNameString().c_str(),
			ipcNameNew.GetModuleNameString().c_str(),
			m_module->m_moduleName.GetModuleNameString().c_str());
}

void IPCSignalHandler::onRemoveIPCObject(const RemoveIPCObjectMessage& msg)
{
	IPCModule::IPCObject object(IPCObjectName::GetIPCName(msg.ipc_name()));
	m_module->m_ipcObject.RemoveObject(object);
	LOG_INFO("Remove IPC Object: ipc name - %s, m_moduleName - %s\n",
		 object.m_ipcName.GetModuleNameString().c_str(), m_module->m_moduleName.GetModuleNameString().c_str());
}

void IPCSignalHandler::onModuleName(const ModuleNameMessage& msg)
{
	IPCModule::IPCObject module(msg.ipc_name(), msg.ip(), msg.port(), msg.access_id());
	const_cast<ModuleNameMessage&>(msg).set_is_exist(!m_module->m_modules.AddObject(module));

	m_module->m_csConnectors.Enter();
	m_module->m_connectors[module.m_ipcName.GetModuleNameString()].push_back(msg.conn_id());
	m_module->m_csConnectors.Leave();
	
	IPCModule::TryConnectCounter counter(module.m_ipcName.GetModuleNameString());
	m_module->m_tryConnectCounters.RemoveObject(counter);
}

void IPCSignalHandler::onDisconnected(const DisconnectedMessage& msg)
{
	bool isModuleDelete = false;
	m_module->m_csConnectors.Enter();
	std::map<std::string, std::vector<std::string> >::iterator it = m_module->m_connectors.find(msg.m_id);
	if(it != m_module->m_connectors.end())
	{
		std::vector<std::string>::iterator it1 = std::find(it->second.begin(), it->second.end(), msg.m_connId);
		if(it1 != it->second.end())
		{
			it->second.erase(it1);
		}

		if(it->second.empty())
		{
			isModuleDelete = true;
			m_module->m_connectors.erase(it);
		}
	}
	m_module->m_csConnectors.Leave();

	if(!isModuleDelete && !m_module->CheckFireConnector(msg.m_id))
	{
		return;
	}

	IPCModule::IPCObject module = IPCObjectName::GetIPCName(msg.m_id);
	m_module->m_modules.RemoveObject(module);

	if (msg.m_id == m_module->m_coordinatorIPCName ||
	   !m_module->m_coordinatorName.empty() && msg.m_id == m_module->m_coordinatorName)
	{
		m_module->m_ipcObject.Clear();
		if(m_module->m_countConnect + 1 == g_ipcCoordinatorPortCount)
		{
			m_module->m_countConnect = 0;
		}
		else
		{
			m_module->m_countConnect++;
		}

		CSLocker locker(&m_module->m_csRequest);
		m_module->m_bConnectToCoordinatorRequest = true;
		time(&m_module->m_requestCreated);
	}
	
	if(msg.m_id != m_module->m_coordinatorIPCName)
	{
		m_module->OnFireConnector(msg.m_id);
	}
}

void IPCSignalHandler::getListenPort(const ListenerParamMessage& msg)
{
	IPCModule::IPCObject object = IPCObjectName::GetIPCName(msg.m_moduleName);
	if(!m_module->m_ipcObject.GetObject(object, &object))
	{
		object = m_module->m_moduleName;
		m_module->m_ipcObject.GetObject(object, &object);
	}

	const_cast<ListenerParamMessage&>(msg).m_moduleName = object.m_ipcName.GetModuleNameString();
	const_cast<ListenerParamMessage&>(msg).m_port = object.m_port;
	const_cast<ListenerParamMessage&>(msg).m_ip = object.m_ip;
	const_cast<ListenerParamMessage&>(msg).m_isCoordinator = (object.m_ipcName.GetModuleNameString() == m_module->m_coordinatorName);
}

void IPCSignalHandler::onIPCObjectList(const IPCObjectListMessage& msg)
{
	std::vector<IPCModule::IPCObject> list = m_module->m_ipcObject.GetObjectList();
	m_module->FillIPCObjectList(const_cast<IPCObjectListMessage&>(msg), list);
}
