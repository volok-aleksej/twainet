#include "tunnel_connector.h"
#include "module/tunnel_module.h"
#include "message/tunnel_messages.h"
#include "common/logger.h"

TunnelConnector::TunnelConnector(AnySocket* socket, const IPCObjectName& moduleName)
: IPCConnector(socket, moduleName), m_isServer(false)
, m_type(UNKNOWN), m_pingThread(0)
{
	addMessage(new ProtoMessage<ModuleName, TunnelConnector>(this));
	addMessage(new ProtoMessage<ModuleState, TunnelConnector>(this));	
	addMessage(new ProtoMessage<InternalConnectionData, TunnelConnector>(this));	
	addMessage(new ProtoMessage<InternalConnectionStatus, TunnelConnector>(this));	
	addMessage(new ProtoMessage<InitInternalConnection, TunnelConnector>(this));	
}

TunnelConnector::~TunnelConnector()
{
	removeReceiver();
}

void TunnelConnector::SetServerFlag()
{
	m_isServer = true;
}

void TunnelConnector::OnStart()
{
	m_pingThread = new PingThread(this);
	if(!m_isServer)
	{
		m_checker = new IPCCheckerThread(this);

		IPCConnector::SetModuleName(IPCObjectName(TunnelModule::m_tunnelIPCName, GetModuleName().module_name()));
		SetId(TunnelModule::m_tunnelIPCName + "." + GetId());

		ProtoMessage<ModuleName> mnMsg(&m_handler);
		*mnMsg.mutable_ipc_name() = GetModuleName();
		mnMsg.set_ip("");
		mnMsg.set_port(0);
		SetAccessId(TunnelModule::m_tunnelAccessId);
		mnMsg.set_access_id(TunnelModule::m_tunnelAccessId);
		toMessage(mnMsg);
	}
}

void TunnelConnector::OnStop()
{
	if(m_pingThread)
	{
		m_pingThread->Stop();
		m_pingThread = 0;
	}
	IPCConnector::OnStop();
}

bool TunnelConnector::SetModuleName(const IPCObjectName& moduleName)
{
	return false;
}

void TunnelConnector::onMessage(const ModuleName& msg)
{
	if(m_isServer)
	{
		LOG_INFO("ModuleName message: m_id-%s, m_module-%s\n", m_id.c_str(), GetModuleName().GetModuleNameString().c_str());
		ModuleNameMessage mnMsg(&m_handler, msg);
		onSignal(mnMsg);
	}
	else
	{
		m_handler.onMessage(msg);
	}
}

void TunnelConnector::onMessage(const ModuleState& msg)
{
	if(m_isServer)
	{
		LOG_INFO("ModuleState message: m_id-%s, m_module-%s, exist - %d\n", m_id.c_str(), GetModuleName().GetModuleNameString().c_str(), msg.exist());
		ModuleStateMessage mnMsg(&m_handler, msg);
		mnMsg.set_id(m_id);
		onSignal(mnMsg);
	}
	else
	{
		m_handler.onMessage(msg);
	}
}

void TunnelConnector::onMessage(const InternalConnectionData& msg)
{
	if(m_isServer)
	{
		InternalConnectionDataMessage icdMsg(&m_handler, msg);
		onSignal(icdMsg);
	}
	else
	{
		m_handler.onMessage(msg);
	}
}

void TunnelConnector::onMessage(const InternalConnectionStatus& msg)
{
	if(m_isServer)
	{
		InternalConnectionStatusMessage icdMsg(&m_handler, msg);
		onSignal(icdMsg);
	}
	else
	{
		m_handler.onMessage(msg);
	}
}

void TunnelConnector::onMessage(const InitInternalConnection& msg)
{
	if(m_isServer)
	{
		InitInternalConnectionMessage icdMsg(&m_handler, msg);
		onSignal(icdMsg);
	}
	else
	{
		m_handler.onMessage(msg);
	}
}

void TunnelConnector::SetTypeConnection(TypeConnection type)
{
	m_type = type;
}

TunnelConnector::TypeConnection TunnelConnector::GetTypeConnection()
{
	return m_type;
}

void TunnelConnector::OnConnected()
{
 	m_bConnected = true;
	TunnelConnectedMessage msg(IPCObjectName::GetIPCName(GetId()).host_name(), m_type);
	onSignal(msg);
}
