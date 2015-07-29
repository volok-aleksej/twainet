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

		ProtoMessage<ModuleName> mnMsg(this);
		*mnMsg.mutable_ipc_name() = GetModuleName();
		mnMsg.set_ip("");
		mnMsg.set_port(0);
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
		ModuleNameMessage mnMsg(this, msg);
		onSignal(mnMsg);
	}
	else
	{
		IPCConnector::onMessage(msg);
	}
}

void TunnelConnector::onMessage(const ModuleState& msg)
{
	if(m_isServer)
	{
		LOG_INFO("ModuleState message: m_id-%s, m_module-%s, exist - %d\n", m_id.c_str(), GetModuleName().GetModuleNameString().c_str(), msg.exist());
		ModuleStateMessage mnMsg(this, msg);
		mnMsg.set_id(m_id);
		onSignal(mnMsg);
	}
	else
	{
		IPCConnector::onMessage(msg);
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
