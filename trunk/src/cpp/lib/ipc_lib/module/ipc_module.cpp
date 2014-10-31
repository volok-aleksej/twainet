#include "ipc_module.h"
#include "connector_lib/thread/connect_thread.h"
#include "connector_lib/socket/socket_factories.h"
#include "thread_lib/thread/thread_manager.h"
#include "common/common.h"

const std::string IPCModule::m_coordinatorIPCName = "IPCCoordinator";
const int IPCModule::m_maxTryConnectCount = 10;

extern std::string CreateGUID();

/*******************************************************************************/
/*                               IPCObject                                     */
/*******************************************************************************/
IPCModule::IPCObject::IPCObject()
: m_port(0), m_ipcName("")
{
}

IPCModule::IPCObject::IPCObject(const IPCObjectName& ipcName, const std::string& ip, int port)
: m_ipcName(ipcName), m_port(port), m_ip(ip)
{
}

IPCModule::IPCObject::~IPCObject()
{
}

void IPCModule::IPCObject::operator=(const IPCObject& object)
{
	m_ipcName = object.m_ipcName;
	m_port = object.m_port;
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
, m_countListener(0), m_count—onnect(0)
{
	m_manager.addSubscriber(this, SIGNAL_FUNC(this, IPCModule, DisconnectedMessage, onDisconnected));
	m_manager.Start();
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
	m_manager.Join();
}

void IPCModule::Start()
{
	m_isCoordinator = false;
	Start("127.0.0.1", 0);
}

void IPCModule::StartAsCoordinator()
{
	m_isCoordinator = true;
	Start("127.0.0.1", g_ipcCoordinatorPorts[m_countListener]);
}

void IPCModule::ConnectTo(const IPCObjectName& moduleName)
{
	TryConnectCounter counter(moduleName.GetModuleNameString());
	if(m_isExit || m_tryConnectCounters.GetObject(counter, &counter))
	{
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
}

void IPCModule::DisconnectModule(const IPCObjectName& moduleName)
{
	m_manager.StopConnection(moduleName.GetModuleNameString());
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
	m_listenThread = new IPCListenThread(address);
	m_listenThread->addSubscriber(this, SIGNAL_FUNC(this, IPCModule, CreatedListenerMessage, onCreatedListener));
	m_listenThread->addSubscriber(this, SIGNAL_FUNC(this, IPCModule, ListenErrorMessage, onErrorListener));
	m_listenThread->addSubscriber(this, SIGNAL_FUNC(this, IPCModule, ConnectorMessage, onAddConnector));
	m_listenThread->Start();
}

void IPCModule::OnNewConnector(Connector* connector)
{
}

void IPCModule::OnConnected(const std::string& moduleName)
{
	printf("\nConnect: %s\n", moduleName.c_str());
}

void IPCModule::OnFireConnector(const std::string& moduleName)
{
	printf("\nDisconnect: %s\n", moduleName.c_str());
}

void IPCModule::OnConnectFailed(const std::string& moduleName)
{
}

bool IPCModule::CheckFireConnector(const std::string& moduleName)
{
	return false;
}

void IPCModule::ConnectToCoordinator()
{
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
	address.m_port = g_ipcCoordinatorPorts[m_count—onnect];
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

	m_moduleName = moduleName;

	ChangeIPCNameMessage cinMsg(0);
	*cinMsg.mutable_ipc_name() = m_moduleName;
	onSignal(cinMsg);

	printf("Update ipc name: %s\n", m_moduleName.GetModuleNameString().c_str());
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
		connector->Subscribe(dynamic_cast<SignalOwner*>(this));
		m_manager.AddConnection(msg.m_conn);
	}
	else
	{
		delete msg.m_conn;
	}
}

void IPCModule::onErrorConnect(const ConnectErrorMessage& msg)
{
	if(msg.m_moduleName == m_coordinatorIPCName)
	{
		if(m_count—onnect + 1 == g_ipcCoordinatorPortCount)
		{
			m_count—onnect = 0;
		}
		else
		{
			m_count—onnect++;
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

		if(counter.m_count < m_maxTryConnectCount)
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
}

void IPCModule::onAddIPCObject(const AddIPCObjectMessage& msg)
{
	IPCObject object(msg.ipc_name(), msg.ip(), msg.port());
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
	std::map<std::string, std::vector<std::string>>::iterator it = m_connectors.find(object.m_ipcName.GetModuleNameString());
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
}

void IPCModule::onRemoveIPCObject(const RemoveIPCObjectMessage& msg)
{
	IPCObject object(msg.ipc_name());
	m_ipcObject.RemoveObject(object);
}

void IPCModule::onModuleName(const ModuleNameMessage& msg)
{
	IPCObject module(msg.ipc_name(), msg.ip(), msg.port());
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
	std::map<std::string, std::vector<std::string>>::iterator it = m_connectors.find(msg.m_id);
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

	if(msg.m_id == m_coordinatorIPCName)
	{
		m_ipcObject.Clear();
		if(m_count—onnect + 1 == g_ipcCoordinatorPortCount)
		{
			m_count—onnect = 0;
		}
		else
		{
			m_count—onnect++;
		}

		Thread::sleep(1000);
		ConnectToCoordinator();
	}
	else
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
}

void IPCModule::onIPCObjectList(const IPCObjectListMessage& msg)
{
	std::vector<IPCObject> list = m_ipcObject.GetObjectList();
	std::vector<IPCObject>::iterator it;
	for(it = list.begin(); it != list.end(); it++)
	{
		AddIPCObject* ipcObject = const_cast<IPCObjectListMessage&>(msg).add_ipc_object();
		ipcObject->set_ip(it->m_ip);
		ipcObject->set_port(it->m_port);
		*ipcObject->mutable_ipc_name() = it->m_ipcName;
	}
}