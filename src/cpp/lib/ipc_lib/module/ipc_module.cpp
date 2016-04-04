#include <stdio.h>
#include <time.h>
#include <algorithm>

#include "ipc_module.h"

#include "common/logger.h"
#include "common/user.h"
#include "common/common.h"
#include "common/common_func.h"

#include "connector_lib/thread/connect_thread.h"
#include "connector_lib/socket/socket_factories.h"
#include "thread_lib/thread/thread_manager.h"

const std::string IPCModule::m_coordinatorIPCName = "IPCCoordinator";
const std::string IPCModule::m_baseAccessId = "Coordinator";
const std::string IPCModule::m_internalAccessId = "Internal";
const int IPCModule::m_maxTryConnectCount = 10;
const int IPCModule::m_connectTimeout = 10;

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
IPCModule::IPCModule(const IPCObjectName& moduleName, ConnectorFactory* factory, int ipv)
: m_moduleName(moduleName), m_factory(factory), m_ipcSignalHandler(this)
, m_isCoordinator(false), m_isExit(false), m_listenThread(0), m_ipv(ipv)
, m_countListener(0), m_countConnect(0), m_bConnectToCoordinatorRequest(false)
{
	ManagersContainer::GetInstance().AddManager(static_cast<IManager*>(this));
	m_manager = new ConnectorManager;
	m_manager->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, DisconnectedMessage, onDisconnected));
}

IPCModule::~IPCModule()
{
	m_isExit = true;

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
	Start("localhost", 0);
}

void IPCModule::StartAsCoordinator()
{
	m_isCoordinator = true;
	m_coordinatorName = m_moduleName.GetModuleNameString();
	Start("localhost", g_ipcCoordinatorPorts[m_countListener]);
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
		address.m_localIP = "localhost";
		address.m_localPort = 0;
		address.m_moduleName = address.m_id = object.m_ipcName.GetModuleNameString();
		address.m_connectorFactory = m_factory->Clone();
		address.m_socketFactory = new TCPSocketFactory(m_ipv);
		address.m_ip = "localhost";
		address.m_port = object.m_port;
		ConnectThread* thread = new ConnectThread(address);
		thread->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, ConnectorMessage, onAddConnector));
		thread->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, ConnectErrorMessage, onErrorConnect));
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
	address.m_socketFactory = new TCPSocketFactory(m_ipv);
	address.m_acceptCount = -1;
	m_listenThread = new BaseListenThread(address);
	m_listenThread->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, CreatedListenerMessage, onCreatedListener));
	m_listenThread->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, ListenErrorMessage, onErrorListener));
	m_listenThread->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, ConnectorMessage, onAddConnector));
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

void IPCModule::FillIPCObjectList(std::vector<IPCObject>& ipcList)
{
}

void IPCModule::OnInternalConnection(const std::string& moduleName, ConnectionStatus status, int port)
{
}

void IPCModule::OnIPCObjectsChanged()
{
}
	
void IPCModule::ManagerFunc()
{
	CSLocker locker(&m_csRequest);
	time_t t;
	time(&t);
	if(m_bConnectToCoordinatorRequest && t - m_requestCreated >= m_connectTimeout)
	{
		ConnectToCoordinator();
		m_bConnectToCoordinatorRequest = false;
	}
}

void IPCModule::ManagerStart()
{
}

void IPCModule::ManagerStop()
{
	m_manager->Stop();
}
	
void IPCModule::ConnectToCoordinator()
{
	LOG_INFO("Try Connect with coordinator: m_moduleName - %s\n", m_moduleName.GetModuleNameString().c_str());
	if(m_isExit)
	{
		return;
	}

	ConnectAddress address;
	address.m_localIP = "localhost";
	address.m_localPort = 0;
	address.m_moduleName = address.m_id = m_coordinatorIPCName;
	address.m_connectorFactory = m_factory->Clone();
	address.m_socketFactory = new TCPSocketFactory(m_ipv);
	address.m_ip = "localhost";
	address.m_port = g_ipcCoordinatorPorts[m_countConnect];
	ConnectThread* thread = new ConnectThread(address);
	thread->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, ConnectorMessage, onAddConnector));
	thread->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, ConnectErrorMessage, onErrorConnect));
	thread->Start();
}

void IPCModule::ipcSubscribe(IPCConnector* connector, SignalReceiver* receiver, IReceiverFunc* func)
{
	connector->addSubscriber(receiver, func);
}

void IPCModule::AddConnector(Connector* conn)
{
	IPCConnector* connector = dynamic_cast<IPCConnector*>(conn);
	if(connector && !m_isExit)
	{
		connector->SetModuleName(m_moduleName);
		connector->SetConnectorId(CreateGUID());

		OnNewConnector(conn);

		connector->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, AddIPCObjectMessage, onAddIPCObject));
		connector->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, UpdateIPCObjectMessage, onUpdateIPCObject));
		connector->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, ModuleNameMessage, onModuleName));
		connector->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, RemoveIPCObjectMessage, onRemoveIPCObject));
		connector->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, IPCObjectListMessage, onIPCObjectList));
		connector->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, ListenerParamMessage, getListenPort));
		connector->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, ConnectedMessage, onConnected));
		connector->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, IPCProtoMessage, onIPCMessage));
		
		connector->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, InternalConnectionStatusMessage, onInternalConnectionStatusMessage));
		
		connector->SubscribeModule(dynamic_cast<SignalOwner*>(this));
		m_manager->AddConnection(conn);
	}
	else
	{
		delete conn;
	}
}
	
void IPCModule::UpdateModuleName(const IPCObjectName& moduleName)
{
	UpdateIPCObjectMessage uioMsg(0);
	*uioMsg.mutable_ipc_old_name() = m_moduleName;
	*uioMsg.mutable_ipc_new_name() = moduleName;
	m_ipcSignalHandler.onUpdateIPCObject(uioMsg);

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
	Stop();
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

std::vector<IPCObjectName> IPCModule::GetConnectedModules()
{
	std::vector<IPCObjectName> retList;
	std::vector<IPCObject> list = m_modules.GetObjectList();
	std::vector<IPCObject>::iterator it;
	for(it = list.begin(); it != list.end(); it++)
	{
		retList.push_back(IPCObjectName(it->m_ipcName));
	}

	return retList;
}
	
std::vector<IPCObjectName> IPCModule::GetInternalConnections()
{
	std::vector<IPCObjectName> retList;
	std::vector<IPCObject> list = m_modules.GetObjectList();
	std::vector<IPCObject>::iterator it;
	for(it = list.begin(); it != list.end(); it++)
	{
		if(!it->m_ipcName.conn_id().empty())
		{
			retList.push_back(IPCObjectName(it->m_ipcName));
		}
	}

	return retList;
}

void IPCModule::CreateInternalConnection(const IPCObjectName& moduleName, const std::string& ip, int port)
{	
	IPCObject object(moduleName);
	if(!m_modules.GetObject(object, &object))
	{
		InitInternalConnectionMessage iicMsg(0);
		iicMsg.set_ip(ip);
		iicMsg.set_port(port);
		iicMsg.mutable_target()->CopyFrom(moduleName);
		onSignal(iicMsg);
	}
	else
	{
		OnInternalConnection(moduleName.GetModuleNameString(), CONN_EXIST, 0);
	}
}