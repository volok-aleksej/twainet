#include <stdio.h>
#include <algorithm>

#include "ipc_module.h"

#include "common/logger.h"
#include "common/common.h"
#include "common/common_func.h"

#include "connector_lib/thread/connect_thread.h"
#include "connector_lib/socket/socket_factories.h"
#include "thread_lib/thread/thread_manager.h"

const std::string IPCModule::m_coordinatorIPCName = "IPCCoordinator";
const std::string IPCModule::m_baseAccessId = "Coordinator";
const int IPCModule::m_maxTryConnectCount = 10;

/*******************************************************************************/
/*                               IPCObject                                     */
/*******************************************************************************/
IPCModule::IPCObject::IPCObject()
: m_port(0), m_ipcName("")
{
}

IPCModule::IPCObject::IPCObject(const IPCObject& object)
	: m_ipcName("")
{
	operator = (object);
}

IPCModule::IPCObject::IPCObject(const IPCObjectName& ipcName, const std::string& ip, int port, const std::string& accessId)
: m_ipcName(ipcName), m_port(port), m_ip(ip), m_accessId(accessId)
{
}

IPCModule::IPCObject::~IPCObject()
{
}

void IPCModule::IPCObject::operator=(const IPCObject& object)
{
	m_ipcName = object.m_ipcName;
	m_port = object.m_port;
	m_ip = object.m_ip;
	m_accessId = object.m_accessId;
}

bool IPCModule::IPCObject::operator != (const IPCObject& object)
{
	return !(operator == (object));
}

bool IPCModule::IPCObject::operator < (const IPCObject& object) const
{
	return m_ipcName < object.m_ipcName;
}

bool IPCModule::IPCObject::operator==(const IPCObject& object)
{
	return m_ipcName == object.m_ipcName;
}

/*******************************************************************************/
/*                             TryConnectCounter                               */
/*******************************************************************************/
IPCModule::TryConnectCounter::TryConnectCounter()
: m_count(0)
{
}

IPCModule::TryConnectCounter::TryConnectCounter(const std::string& moduleName)
: m_moduleName(moduleName), m_count(0){}

bool IPCModule::TryConnectCounter::operator == (const TryConnectCounter& counter)
{
	return m_moduleName == counter.m_moduleName;
}

bool IPCModule::TryConnectCounter::operator != (const TryConnectCounter& counter)
{
	return m_moduleName != counter.m_moduleName;
}

bool IPCModule::TryConnectCounter::operator < (const TryConnectCounter& counter) const
{
	return m_moduleName < counter.m_moduleName;
}

void IPCModule::TryConnectCounter::operator = (const TryConnectCounter& counter)
{
	m_moduleName = counter.m_moduleName;
	m_count = counter.m_count;
}

/*******************************************************************************/
/*                               IPCModule                                     */
/*******************************************************************************/
IPCModule::IPCModule(const IPCObjectName& moduleName, ConnectorFactory* factory)
: m_moduleName(moduleName), m_factory(factory)
, m_isCoordinator(false), m_isExit(false), m_listenThread(0)
, m_countListener(0), m_countConnect(0)
{
	m_manager = new ConnectorManager;
	m_manager->addSubscriber(this, SIGNAL_FUNC(this, IPCModule, DisconnectedMessage, onDisconnected));
}

IPCModule::~IPCModule()
{
	m_isExit = true;
	removeReceiver();

	if(m_listenThread)
	{
		m_listenThread->Stop();
		ThreadManager::GetInstance().AddThread(m_listenThread);
		m_listenThread = 0;
	}
	delete m_factory;
}

void IPCModule::Start()
{
	m_isCoordinator = false;
	Start("127.0.0.1", 0);
}

void IPCModule::StartAsCoordinator()
{
	m_isCoordinator = true;
	m_coordinatorName = m_moduleName.GetModuleNameString();
	Start("127.0.0.1", g_ipcCoordinatorPorts[m_countListener]);
}

void IPCModule::ConnectTo(const IPCObjectName& moduleName)
{
	LOG_INFO("Try Connect with %s: m_moduleName - %s\n", const_cast<IPCObjectName&>(moduleName).GetModuleNameString().c_str(), m_moduleName.GetModuleNameString().c_str());
	IPCObjectName module(moduleName);
	TryConnectCounter counter(module.GetModuleNameString());
	if(m_isExit || m_tryConnectCounters.GetObject(counter, &counter))
	{
		OnConnectFailed(const_cast<IPCObjectName&>(moduleName).GetModuleNameString());
		return;
	}

	IPCObject object(moduleName);
	if(m_ipcObject.GetObject(object, &object) && !m_modules.GetObject(object, &object))
	{
		ConnectAddress address;
		address.m_localIP = "127.0.0.1";
		address.m_localPort = 0;
		address.m_moduleName = address.m_id = object.m_ipcName.GetModuleNameString();
		address.m_connectorFactory = m_factory->Clone();
		address.m_socketFactory = new TCPSocketFactory;
		address.m_ip = "127.0.0.1";
		address.m_port = object.m_port;
		ConnectThread* thread = new ConnectThread(address);
		thread->addSubscriber(this, SIGNAL_FUNC(this, IPCModule, ConnectorMessage, onAddConnector));
		thread->addSubscriber(this, SIGNAL_FUNC(this, IPCModule, ConnectErrorMessage, onErrorConnect));
		m_tryConnectCounters.AddObject(counter);
		thread->Start();
	}
	else
	{
		OnConnectFailed(const_cast<IPCObjectName&>(moduleName).GetModuleNameString());
	}
}

void IPCModule::DisconnectModule(const IPCObjectName& moduleName)
{
	LOG_INFO("Try disconnect from %s: m_moduleName - %s\n", const_cast<IPCObjectName&>(moduleName).GetModuleNameString().c_str(), m_moduleName.GetModuleNameString().c_str());
	IPCObjectName module(moduleName);
	m_manager->StopConnection(module.GetModuleNameString());
}

void IPCModule::Start(const std::string& ip, int port)
{
	if(m_listenThread)
	{
		m_listenThread->Stop();
		ThreadManager::GetInstance().AddThread(m_listenThread);
		m_listenThread = 0;
	}

	ListenAddress address;
	address.m_id = m_moduleName.GetModuleNameString();
	address.m_localIP = ip;
	address.m_localPort = port;
	address.m_connectorFactory = m_factory->Clone();
	address.m_socketFactory = new TCPSocketFactory;
	address.m_acceptCount = -1;
	m_listenThread = new BaseListenThread(address);
	m_listenThread->addSubscriber(this, SIGNAL_FUNC(this, IPCModule, CreatedListenerMessage, onCreatedListener));
	m_listenThread->addSubscriber(this, SIGNAL_FUNC(this, IPCModule, ListenErrorMessage, onErrorListener));
	m_listenThread->addSubscriber(this, SIGNAL_FUNC(this, IPCModule, ConnectorMessage, onAddConnector));
	m_listenThread->Start();
}

void IPCModule::OnNewConnector(Connector* connector)
{
	LOG_INFO("New connector created: m_id - %s, m_moduleName - %s\n", connector->GetId().c_str(), m_moduleName.GetModuleNameString().c_str());
}

void IPCModule::OnConnected(const std::string& moduleName)
{
	LOG_INFO("Connector created: moduleName - %s, m_moduleName - %s\n", moduleName.c_str(), m_moduleName.GetModuleNameString().c_str());
}

void IPCModule::OnFireConnector(const std::string& moduleName)
{
	LOG_INFO("Connector destroyed: moduleName - %s, m_moduleName - %s\n", moduleName.c_str(), m_moduleName.GetModuleNameString().c_str());
}

void IPCModule::OnConnectFailed(const std::string& moduleName)
{
	LOG_INFO("Connection failed: moduleName - %s, m_moduleName - %s\n", moduleName.c_str(), m_moduleName.GetModuleNameString().c_str());
}

void IPCModule::OnMessage(const std::string& messageName, const std::vector<std::string>& path, const std::string& data)
{
}

bool IPCModule::CheckFireConnector(const std::string& moduleName)
{
	return false;
}

void IPCModule::ModuleCreationFialed()
{
	LOG_INFO("Module Creation Failed: m_moduleName - %s\n", m_moduleName.GetModuleNameString().c_str());
}

void IPCModule::FillIPCObjectList(IPCObjectListMessage& msg)
{
	std::vector<IPCObject> list = m_ipcObject.GetObjectList();
	std::vector<IPCObject>::iterator it;
	for(it = list.begin(); it != list.end(); it++)
	{
		AddIPCObject* ipcObject = const_cast<IPCObjectListMessage&>(msg).add_ipc_object();
		ipcObject->set_ip(it->m_ip);
		ipcObject->set_port(it->m_port);
		ipcObject->set_access_id(it->m_accessId);
		*ipcObject->mutable_ipc_name() = it->m_ipcName;
	}
}

void IPCModule::OnInternalConnection(const std::string& moduleName, const std::string& id, ConnectionStatus status, int port)
{
}

void IPCModule::ConnectToCoordinator()
{
	LOG_INFO("Try Connect with coordinator: m_moduleName - %s\n", m_moduleName.GetModuleNameString().c_str());
	if(m_isExit)
	{
		return;
	}

	ConnectAddress address;
	address.m_localIP = "127.0.0.1";
	address.m_localPort = 0;
	address.m_moduleName = address.m_id = m_coordinatorIPCName;
	address.m_connectorFactory = m_factory->Clone();
	address.m_socketFactory = new TCPSocketFactory;
	address.m_ip = "127.0.0.1";
	address.m_port = g_ipcCoordinatorPorts[m_countConnect];
	ConnectThread* thread = new ConnectThread(address);
	thread->addSubscriber(this, SIGNAL_FUNC(this, IPCModule, ConnectorMessage, onAddConnector));
	thread->addSubscriber(this, SIGNAL_FUNC(this, IPCModule, ConnectErrorMessage, onErrorConnect));
	thread->Start();
}

void IPCModule::ipcSubscribe(IPCConnector* connector, IReceiverFunc* func)
{
	connector->addSubscriber(this, func);
}

void IPCModule::UpdateModuleName(const IPCObjectName& moduleName)
{
	UpdateIPCObjectMessage uioMsg(0);
	*uioMsg.mutable_ipc_old_name() = m_moduleName;
	*uioMsg.mutable_ipc_new_name() = moduleName;
	onUpdateIPCObject(uioMsg);

	LOG_INFO("Update ipc name: old - %s, new - %s\n", m_moduleName.GetModuleNameString().c_str(), const_cast<IPCObjectName&>(moduleName).GetModuleNameString().c_str());
	m_moduleName = moduleName;

	ChangeIPCNameMessage cinMsg(0);
	*cinMsg.mutable_ipc_name() = m_moduleName;
	onSignal(cinMsg);

}

void IPCModule::SendMsg(const IPCMessageSignal& msg)
{
	onSignal(msg);
}

const IPCObjectName& IPCModule::GetModuleName()
{
	return m_moduleName;
}

bool IPCModule::IsExit()
{
	return m_isExit;
}

void IPCModule::Exit()
{
	m_isExit = true;
}

void IPCModule::onCreatedListener(const CreatedListenerMessage& msg)
{
	IPCObject object(IPCObjectName::GetIPCName(msg.m_id), msg.m_ip, msg.m_port);
	m_ipcObject.AddObject(object);
	if(!m_isCoordinator)
	{
		ConnectToCoordinator();
	}

	LOG_INFO("Module created: m_moduleName - %s\n", m_moduleName.GetModuleNameString().c_str());
}

void IPCModule::onErrorListener(const ListenErrorMessage& msg)
{
	if(m_isCoordinator && m_countListener + 1 < g_ipcCoordinatorPortCount)
	{
		m_countListener++;
		StartAsCoordinator();
	}
	else
	{
		ThreadManager::GetInstance().AddThread(m_listenThread);
		m_listenThread = 0;
		m_isExit = true;
		ModuleCreationFialed();
	}
}

void IPCModule::onAddConnector(const ConnectorMessage& msg)
{
	IPCConnector* connector = dynamic_cast<IPCConnector*>(msg.m_conn);
	if(connector && !m_isExit)
	{
		connector->SetModuleName(m_moduleName);
		connector->SetConnectorId(CreateGUID());
	
		OnNewConnector(msg.m_conn);

		connector->addSubscriber(this, SIGNAL_FUNC(this, IPCModule, AddIPCObjectMessage, onAddIPCObject));
		connector->addSubscriber(this, SIGNAL_FUNC(this, IPCModule, UpdateIPCObjectMessage, onUpdateIPCObject));
		connector->addSubscriber(this, SIGNAL_FUNC(this, IPCModule, ModuleNameMessage, onModuleName));
		connector->addSubscriber(this, SIGNAL_FUNC(this, IPCModule, RemoveIPCObjectMessage, onRemoveIPCObject));
		connector->addSubscriber(this, SIGNAL_FUNC(this, IPCModule, IPCObjectListMessage, onIPCObjectList));
		connector->addSubscriber(this, SIGNAL_FUNC(this, IPCModule, ListenerParamMessage, getListenPort));
		connector->addSubscriber(this, SIGNAL_FUNC(this, IPCModule, ConnectedMessage, onConnected));
		connector->addSubscriber(this, SIGNAL_FUNC(this, IPCModule, IPCProtoMessage, onIPCMessage));
		
		connector->addSubscriber(this, SIGNAL_FUNC(this, IPCModule, InternalConnectionStatusMessage, onInternalConnectionStatusMessage));
		
		connector->SubscribeModule(dynamic_cast<SignalOwner*>(this));
		m_manager->AddConnection(msg.m_conn);
	}
	else
	{
		delete msg.m_conn;
	}
}

void IPCModule::onErrorConnect(const ConnectErrorMessage& msg)
{
	if(msg.m_moduleName == m_coordinatorIPCName && !m_isExit)
	{
		if(m_countConnect + 1 == g_ipcCoordinatorPortCount)
		{
			m_countConnect = 0;
		}
		else
		{
			m_countConnect++;
		}

		ConnectToCoordinator();
	}
	else
	{
		TryConnectCounter counter(msg.m_moduleName);
		if(!m_tryConnectCounters.GetObject(counter, &counter))
		{
			OnConnectFailed(msg.m_moduleName);
		}

		if(counter.m_count < m_maxTryConnectCount && !m_isExit)
		{
			ConnectTo(IPCObjectName::GetIPCName(msg.m_moduleName));
			counter.m_count++;
			m_tryConnectCounters.UpdateObject(counter);
		}
		else
		{
			OnConnectFailed(msg.m_moduleName);
			m_tryConnectCounters.RemoveObject(counter);
		}
	}
}

void IPCModule::onConnected(const ConnectedMessage& msg)
{
	OnConnected(msg.m_id);
	if(msg.m_bWithCoordinator)
	{
		LOG_INFO("Module connected with coordinator: coordinator name - %s, m_moduleName - %s\n", m_coordinatorName.c_str(), m_moduleName.GetModuleNameString().c_str());
		m_coordinatorName = msg.m_id;
	}
}

void IPCModule::onInternalConnectionStatusMessage(const InternalConnectionStatusMessage& msg)
{
	IPCObjectName target(msg.target());
	OnInternalConnection(target.GetModuleNameString(), msg.id(), msg.status(), msg.port());
}

void IPCModule::onIPCMessage(const IPCProtoMessage& msg)
{
	std::vector<std::string> path;
	for(int i = 0; i < msg.ipc_sender_size(); i++)
	{
		IPCObjectName sender(msg.ipc_sender(i));
		path.push_back(sender.GetModuleNameString());
	}
	OnMessage(msg.message_name(), path, msg.message());
}

void IPCModule::onAddIPCObject(const AddIPCObjectMessage& msg)
{
	IPCObject object(msg.ipc_name(), msg.ip(), msg.port(), msg.access_id());
	LOG_INFO("Add IPC Object: ipc name - %s, m_moduleName - %s\n", object.m_ipcName.GetModuleNameString().c_str(), m_moduleName.GetModuleNameString().c_str());
	m_ipcObject.AddObject(object);
}

void IPCModule::onUpdateIPCObject(const UpdateIPCObjectMessage& msg)
{
	IPCObject object(msg.ipc_old_name());
	m_ipcObject.GetObject(object, &object);
	m_ipcObject.RemoveObject(object);
	bool update = false;
	if(m_modules.RemoveObject(object))
	{
		update = true;
	}

	m_csConnectors.Enter();
	std::map<std::string, std::vector<std::string> >::iterator it = m_connectors.find(object.m_ipcName.GetModuleNameString());
	if(it != m_connectors.end())
	{
		IPCObjectName newName(msg.ipc_new_name());
		std::vector<std::string> connectors = it->second;
		m_connectors.erase(it);
		m_connectors.insert(std::make_pair(newName.GetModuleNameString(), connectors));
	}
	m_csConnectors.Leave();

	TryConnectCounter counter(object.m_ipcName.GetModuleNameString());
	if(m_tryConnectCounters.GetObject(counter, &counter))
	{
		m_tryConnectCounters.RemoveObject(counter);
		IPCObjectName ipcName(msg.ipc_new_name());
		counter.m_moduleName = ipcName.GetModuleNameString();
		m_tryConnectCounters.AddObject(counter);
	}

	object.m_ipcName = msg.ipc_new_name();

	m_ipcObject.AddObject(object);
	if(update)
	{
		m_modules.AddObject(object);
	}

	IPCObjectName ipcNameNew(msg.ipc_new_name());
	IPCObjectName ipcNameOld(msg.ipc_old_name());
	LOG_INFO("Update IPC Object: ipc name old - %s, ipc name new - %s, m_moduleName - %s\n",
			ipcNameOld.GetModuleNameString().c_str(),
			ipcNameNew.GetModuleNameString().c_str(),
			m_moduleName.GetModuleNameString().c_str());
}

void IPCModule::onRemoveIPCObject(const RemoveIPCObjectMessage& msg)
{
	IPCObject object(IPCObjectName::GetIPCName(msg.ipc_name()));
	m_ipcObject.RemoveObject(object);
	LOG_INFO("Remove IPC Object: ipc name - %s, m_moduleName - %s\n", object.m_ipcName.GetModuleNameString().c_str(), m_moduleName.GetModuleNameString().c_str());
}

void IPCModule::onModuleName(const ModuleNameMessage& msg)
{
	IPCObject module(msg.ipc_name(), msg.ip(), msg.port(), msg.access_id());
	const_cast<ModuleNameMessage&>(msg).set_is_exist(!m_modules.AddObject(module));

	m_csConnectors.Enter();
	m_connectors[module.m_ipcName.GetModuleNameString()].push_back(msg.conn_id());
	m_csConnectors.Leave();
	
	TryConnectCounter counter(module.m_ipcName.GetModuleNameString());
	m_tryConnectCounters.RemoveObject(counter);
}

void IPCModule::onDisconnected(const DisconnectedMessage& msg)
{
	bool isModuleDelete = false;
	m_csConnectors.Enter();
	std::map<std::string, std::vector<std::string> >::iterator it = m_connectors.find(msg.m_id);
	if(it != m_connectors.end())
	{
		std::vector<std::string>::iterator it1 = std::find(it->second.begin(), it->second.end(), msg.m_connId);
		if(it1 != it->second.end())
		{
			it->second.erase(it1);
		}

		if(it->second.empty())
		{
			isModuleDelete = true;
			m_connectors.erase(it);
		}
	}
	m_csConnectors.Leave();

	if(!isModuleDelete && !CheckFireConnector(msg.m_id))
	{
		return;
	}

	IPCObject module = IPCObjectName::GetIPCName(msg.m_id);
	m_modules.RemoveObject(module);

	if (msg.m_id == m_coordinatorIPCName ||
		!m_coordinatorName.empty() && msg.m_id == m_coordinatorName)
	{
		m_ipcObject.Clear();
		if(m_countConnect + 1 == g_ipcCoordinatorPortCount)
		{
			m_countConnect = 0;
		}
		else
		{
			m_countConnect++;
		}

		Thread::sleep(1000);
		ConnectToCoordinator();
	}
	
	if(msg.m_id != m_coordinatorIPCName)
	{
		OnFireConnector(msg.m_id);
	}
}

void IPCModule::getListenPort(const ListenerParamMessage& msg)
{
	IPCObject object = IPCObjectName::GetIPCName(msg.m_moduleName);
	if(!m_ipcObject.GetObject(object, &object))
	{
		object = m_moduleName;
		m_ipcObject.GetObject(object, &object);
	}

	const_cast<ListenerParamMessage&>(msg).m_moduleName = object.m_ipcName.GetModuleNameString();
	const_cast<ListenerParamMessage&>(msg).m_port = object.m_port;
	const_cast<ListenerParamMessage&>(msg).m_ip = object.m_ip;
	const_cast<ListenerParamMessage&>(msg).m_isCoordinator = (object.m_ipcName.GetModuleNameString() == m_coordinatorName);
}

void IPCModule::onIPCObjectList(const IPCObjectListMessage& msg)
{
	FillIPCObjectList(const_cast<IPCObjectListMessage&>(msg));
}

std::vector<IPCObjectName> IPCModule::GetIPCObjects()
{
	std::vector<IPCObjectName> retList;
	std::vector<IPCObject> list = m_ipcObject.GetObjectList();
	std::vector<IPCObject>::iterator it;
	for(it = list.begin(); it != list.end(); it++)
	{
		retList.push_back(IPCObjectName(it->m_ipcName));
	}

	return retList;
}


void IPCModule::CreateInternalConnection(const IPCObjectName& moduleName, const std::string& ip, int port, const std::string& id)
{
	InitInternalConnectionMessage iicMsg(0);
	iicMsg.set_id(id);
	iicMsg.set_ip(ip);
	iicMsg.set_port(port);
	iicMsg.mutable_target()->CopyFrom(moduleName);
	onSignal(iicMsg);
}