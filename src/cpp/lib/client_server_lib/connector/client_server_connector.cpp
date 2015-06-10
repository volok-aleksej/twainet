#include "client_server_connector.h"
#include "message/client_server_messages.h"
#include "module/client_server_module.h"
#include "common/guid_generator.h"

class ClientSendMessage : public DataMessage
{
public:
	ClientSendMessage(const IPCMessage& message)
		: m_ipcMsg(message){}

	virtual std::string GetDeserializeName()const
	{
		return m_ipcMsg.GetTypeName();
	}

	virtual bool deserialize(char* data, int& len)
	{
		int size = m_ipcMsg.message().size();
		if (size > len)
		{
			len = size;
			return false;
		}

		len = size;
		memcpy(data, m_ipcMsg.message().c_str(), m_ipcMsg.message().size());
		return true;
	}
private:
	IPCMessage m_ipcMsg;
};

ClientServerConnector::ClientServerConnector(AnySocket* socket, const IPCObjectName& moduleName)
: IPCConnector(socket, moduleName)
{
	addMessage(new LoginMessage(this));
	addMessage(new LoginResultMessage(this));
	addMessage(new InitTunnelMessage(this));
	addMessage(new TryConnectToMessage(this));
	addMessage(new InitTunnelStartedMessage(this));
	addMessage(new PeerDataMessage(this));

	m_isNotifyRemove = true;
	m_isCoordinator = false;
}

ClientServerConnector::~ClientServerConnector()
{
	removeReceiver();
}

void ClientServerConnector::onNewConnector(const Connector* connector)
{
	IPCConnector* conn = const_cast<IPCConnector*>(static_cast<const IPCConnector*>(connector));
	if(conn)
	{
		ipcSubscribe(conn, SIGNAL_FUNC(this, ClientServerConnector, IPCProtoMessage, onIPCMessage));
	}
	ClientServerConnector* csConn = dynamic_cast<ClientServerConnector*>(const_cast<Connector*>(connector));
	if(csConn)
	{
		ipcSubscribe(csConn, SIGNAL_FUNC(this, ClientServerConnector, TryConnectToMessage, onTryConnectToMessage));
		addIPCSubscriber(csConn, SIGNAL_FUNC(csConn, ClientServerConnector, TryConnectToMessage, onTryConnectToMessage));
		addIPCSubscriber(csConn, SIGNAL_FUNC(csConn, ClientServerConnector, IPCProtoMessage, onIPCMessage));
	}
}

void ClientServerConnector::Subscribe(::SignalOwner* owner)
{
	owner->addSubscriber(this, SIGNAL_FUNC(this, ClientServerConnector, InitTunnelSignal, onInitTunnelSignal));
	owner->addSubscriber(this, SIGNAL_FUNC(this, ClientServerConnector, InitTunnelStartedSignal, onInitTunnelStartedSignal));
	owner->addSubscriber(this, SIGNAL_FUNC(this, ClientServerConnector, TryConnectToSignal, onTryConnectToSignal));
	owner->addSubscriber(this, SIGNAL_FUNC(this, ClientServerConnector, PeerDataSignal, onPeerDataSignal));
	IPCConnector::Subscribe(owner);
}

void ClientServerConnector::SetUserName(const std::string& userName)
{
	m_userName = userName;
}

void ClientServerConnector::SetPassword(const std::string& password)
{
	m_password = password;
}

void ClientServerConnector::OnStart()
{
	m_checker = new IPCCheckerThread(this);
	if(m_id == ClientServerModule::m_serverIPCName)
	{
		ProtoMessage<Login, ClientServerConnector> loginMsg(this);
		loginMsg.set_name(m_userName);
		loginMsg.set_pasword(m_password);
		toMessage(loginMsg);
	}
}

bool ClientServerConnector::SetModuleName(const IPCObjectName& moduleName)
{
	return false;
}

void ClientServerConnector::OnConnected()
{
 	m_bConnected = true;
	ClientServerConnectedMessage msg(GetId());
	onSignal(msg);
}

IPCObjectName ClientServerConnector::GetIPCName()
{
	IPCObjectName name = IPCObjectName::GetIPCName(GetId());
	name.set_host_name(m_ownSessionId);
	return name;
}

void ClientServerConnector::onIPCMessage(const IPCProtoMessage& msg)
{
	IPCObjectName path(msg.ipc_path(0));
	if (m_ownSessionId == path.GetModuleNameString() &&
		GetModuleName().module_name() == ClientServerModule::m_serverIPCName)
	{
		IPCProtoMessage newMsg(msg);
		newMsg.clear_ipc_path();
		IPCObjectName server(ClientServerModule::m_clientIPCName, m_ownSessionId);
		*newMsg.add_ipc_path() = server;
		for(int i = 1; i < msg.ipc_path_size(); i++)
		{
			*newMsg.add_ipc_path() = msg.ipc_path(i);
		}
		toMessage(newMsg);
	}
}

void ClientServerConnector::onInitTunnelSignal(const InitTunnelSignal& msg)
{
	IPCObjectName idName = IPCObjectName::GetIPCName(m_id);
	if((idName.module_name() == ClientServerModule::m_clientIPCName ||
		!msg.has_type() && idName.module_name() == ClientServerModule::m_serverIPCName)
		&& msg.own_session_id() == m_ownSessionId)
	{
		InitTunnelMessage itMsg(this, msg);
		toMessage(itMsg);
	}
}


void ClientServerConnector::onInitTunnelStartedSignal(const InitTunnelStartedSignal& msg)
{
	IPCObjectName idName = IPCObjectName::GetIPCName(m_id);
	if(idName.module_name() == ClientServerModule::m_clientIPCName
		&& msg.own_session_id() == m_ownSessionId)
	{
		InitTunnelStartedMessage itrMsg(this, msg);
		toMessage(itrMsg);
	}
}

void ClientServerConnector::onTryConnectToMessage(const TryConnectToMessage& msg)
{
	IPCObjectName idName = IPCObjectName::GetIPCName(m_id);
	if(idName.module_name() == ClientServerModule::m_clientIPCName
		&& msg.own_session_id() == m_ownSessionId)
	{
		toMessage(msg);
	}
}

void ClientServerConnector::onTryConnectToSignal(const TryConnectToSignal& msg)
{
	IPCObjectName idName = IPCObjectName::GetIPCName(m_id);
	if(((idName.module_name() == ClientServerModule::m_serverIPCName
		&& msg.type() == TUNNEL_LOCAL)
		|| (idName.module_name() == ClientServerModule::m_clientIPCName
		&& msg.type() == TUNNEL_EXTERNAL))
		&& msg.own_session_id() == m_ownSessionId)
	{
		TryConnectToMessage tctMsg(this, msg);
		toMessage(tctMsg);
	}	
}

void ClientServerConnector::onPeerDataSignal(const PeerDataSignal& msg)
{
	IPCObjectName idName = IPCObjectName::GetIPCName(m_id);
	if(idName.module_name() == ClientServerModule::m_serverIPCName
		&& msg.one_session_id() == m_ownSessionId)
	{
		PeerDataMessage pdMsg(this, msg);
		toMessage(pdMsg);
	}
}

void ClientServerConnector::onMessage(const LoginResult& msg)
{
	if(m_checker)
	{
		delete m_checker;
		m_checker = 0;
	}
	m_ownSessionId = msg.own_session_id();

	LoginResultMessage lrMsg(this, msg);
	onSignal(lrMsg);
	
	IPCObjectName name(GetId(), m_ownSessionId);
	SetId(name.GetModuleNameString());
	IPCConnector::SetModuleName(IPCObjectName(GetModuleName().module_name(), m_ownSessionId));
	
	ProtoMessage<ModuleName> mnMsg(this);
	*mnMsg.mutable_ipc_name() = GetModuleName();
	mnMsg.set_ip("");
	mnMsg.set_port(0);
	toMessage(mnMsg);
}

void ClientServerConnector::onMessage(const Login& msg)
{
	if(m_checker)
	{
		delete m_checker;
		m_checker = 0;
	}

	ProtoMessage<LoginResult, ClientServerConnector> loginResultMsg(this);
	loginResultMsg.set_login_result(LOGIN_SUCCESS);
	loginResultMsg.set_own_session_id(CreateGUID());
	toMessage(loginResultMsg);

	m_ownSessionId = loginResultMsg.own_session_id();
	
	IPCObjectName name(GetId(), m_ownSessionId);
	SetId(name.GetModuleNameString());
	IPCConnector::SetModuleName(IPCObjectName(GetModuleName().module_name(), m_ownSessionId));
	
	ProtoMessage<ModuleName> mnMsg(this);
	*mnMsg.mutable_ipc_name() = GetModuleName();
	mnMsg.set_ip("");
	mnMsg.set_port(0);
	toMessage(mnMsg);
}

void ClientServerConnector::onMessage(const PeerData& msg)
{
	PeerDataSignal pdSig(msg);
	onSignal(pdSig);
}

void ClientServerConnector::onMessage(const InitTunnel& msg)
{
	IPCObjectName idName = IPCObjectName::GetIPCName(m_id);
	if(idName.module_name() == ClientServerModule::m_clientIPCName)
	{
		InitTunnelSignal itMsg(msg);
		onSignal(itMsg);
	}
	else if(idName.module_name() == ClientServerModule::m_serverIPCName)
	{
		InitTunnelMessage itMsg(this, msg);
		onSignal(itMsg);
	}
}

void ClientServerConnector::onMessage(const TryConnectTo& msg)
{
	TryConnectToMessage tctMsg(this, msg);
	IPCObjectName idName = IPCObjectName::GetIPCName(m_id);
	if(idName.module_name() == ClientServerModule::m_clientIPCName)
	{
		tctMsg.set_own_session_id(msg.ext_session_id());
		tctMsg.set_ext_session_id(msg.own_session_id());
		onIPCSignal(tctMsg);
	}
	else if(idName.module_name() == ClientServerModule::m_serverIPCName)
	{
		onSignal(tctMsg);
	}
}

void ClientServerConnector::onMessage(const InitTunnelStarted& msg)
{
	InitTunnelStartedMessage itrMsg(this, msg);
	onSignal(itrMsg);
}
