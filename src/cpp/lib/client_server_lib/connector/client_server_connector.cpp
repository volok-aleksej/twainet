#include "client_server_connector.h"
#include "message/client_server_messages.h"
#include "module/client_server_module.h"
#include "common/common_func.h"
#include "utils/logger.h"

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
	addMessage(new InitTunnelCompleteMessage(this));
	addMessage(new PeerDataMessage(this));
    addMessage(new AvailablePearTypesMessage(this));
	addMessage(new ProtoMessage<ModuleName, ClientServerConnector>(this));
}

ClientServerConnector::~ClientServerConnector()
{
	removeReceiver();
}

void ClientServerConnector::SubscribeConnector(const IPCConnector* connector)
{
	IPCConnector* ipcConn = const_cast<IPCConnector*>(connector);
	if(ipcConn)
	{
		ipcSubscribe(ipcConn, SIGNAL_FUNC(this, ClientServerConnector, IPCProtoMessage, onIPCMessage));
	}
	ClientServerConnector* conn = dynamic_cast<ClientServerConnector*>(ipcConn);
	if(conn)
	{
		ipcSubscribe(conn, SIGNAL_FUNC(this, ClientServerConnector, TryConnectToMessage, onTryConnectToMessage));
		ipcSubscribe(conn, SIGNAL_FUNC(this, ClientServerConnector, AddIPCObjectMessage, onAddIPCObjectMessage));
		ipcSubscribe(conn, SIGNAL_FUNC(this, ClientServerConnector, IPCObjectListMessage, onIPCObjectListMessage));
		ipcSubscribe(ipcConn, SIGNAL_FUNC(this, ClientServerConnector, RemoveIPCObjectMessage, onRemoveIPCObjectMessage));
	}
}

void ClientServerConnector::SubscribeModule(::SignalOwner* owner)
{
	owner->addSubscriber(this, SIGNAL_FUNC(this, ClientServerConnector, InitTunnelSignal, onInitTunnelSignal));
	owner->addSubscriber(this, SIGNAL_FUNC(this, ClientServerConnector, InitTunnelStartedSignal, onInitTunnelStartedSignal));
	owner->addSubscriber(this, SIGNAL_FUNC(this, ClientServerConnector, InitTunnelCompleteSignal, onInitTunnelCompleteSignal));
	owner->addSubscriber(this, SIGNAL_FUNC(this, ClientServerConnector, TryConnectToSignal, onTryConnectToSignal));
	owner->addSubscriber(this, SIGNAL_FUNC(this, ClientServerConnector, PeerDataSignal, onPeerDataSignal));
    owner->addSubscriber(this, SIGNAL_FUNC(this, ClientServerConnector, AvailablePearTypesSignal, onAvailablePearTypesSignal));
	IPCConnector::SubscribeModule(owner);
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
		loginMsg.set_password(m_password);
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
	if(path == GetModuleName())
	{
		IPCProtoMessage newMsg(msg);
		newMsg.clear_ipc_path();
		IPCObjectName client = IPCObjectName::GetIPCName(m_id);
		*newMsg.add_ipc_path() = client;
		for(int i = 1; i < msg.ipc_path_size(); i++)
		{
			*newMsg.add_ipc_path() = msg.ipc_path(i);
		}
		toMessage(newMsg);
	}
	else if(path.GetModuleNameString() == m_id)
	{
		toMessage(msg);
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

void ClientServerConnector::onInitTunnelCompleteSignal(const InitTunnelCompleteSignal& msg)
{
	IPCObjectName idName = IPCObjectName::GetIPCName(m_id);
	if(idName.module_name() == ClientServerModule::m_serverIPCName
		&& msg.own_session_id() == m_ownSessionId)
	{
		InitTunnelCompleteMessage itrMsg(this, msg);
		toMessage(itrMsg);
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
		&& (msg.type() == TUNNEL_LOCAL_TCP || msg.type() == TUNNEL_LOCAL_UDP))
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

void ClientServerConnector::onAvailablePearTypesSignal(const AvailablePearTypesSignal& msg)
{
    IPCObjectName idName = IPCObjectName::GetIPCName(m_id);
    if(idName.module_name() == ClientServerModule::m_serverIPCName
        && msg.session_id() == m_ownSessionId)
    {
        AvailablePearTypesMessage pdMsg(this, msg);
        toMessage(pdMsg);
    }   
}

void ClientServerConnector::onIPCObjectListMessage(const IPCObjectListMessage& msg)
{
	IPCObjectName idName = IPCObjectName::GetIPCName(m_id);
	if(idName.module_name() == ClientServerModule::m_clientIPCName && msg.access_id() == GetAccessId())
	{
		AddIPCObject* ipcObject = const_cast<IPCObjectListMessage&>(msg).add_ipc_object();
		ipcObject->set_ip("");
		ipcObject->set_port(0);
		ipcObject->set_access_id(GetAccessId());
		*ipcObject->mutable_ipc_name() = idName;
	}
}

void ClientServerConnector::onAddIPCObjectMessage(const AddIPCObjectMessage& msg)
{
	IPCObjectName idName = IPCObjectName::GetIPCName(m_id);
	if(idName.module_name() == ClientServerModule::m_clientIPCName && msg.access_id() == GetAccessId())
	{
		toMessage(msg);
	}
}

void ClientServerConnector::onRemoveIPCObjectMessage(const RemoveIPCObjectMessage& msg)
{
	IPCConnector::onRemoveIPCObjectMessage(msg);
}

void ClientServerConnector::onMessage(const LoginResult& msg)
{
	if(m_checker)
	{
		m_checker->Stop();
		m_checker = 0;
	}
	

	LoginResultMessage lrMsg(this, msg);
	onSignal(lrMsg);
	if(msg.login_result() == LOGIN_FAILURE)
	{
		LOG_INFO("Login failed: m_moduleName - %s\n", GetModuleName().GetModuleNameString().c_str());
		Stop();
		return;
	}

	m_ownSessionId = msg.own_session_id();
	SetAccessId(m_userName);

	IPCObjectName name(GetId(), m_ownSessionId);
	SetId(name.GetModuleNameString());
	IPCConnector::SetModuleName(IPCObjectName(GetModuleName().module_name(), m_ownSessionId));
	
	LOG_INFO("Login succesful: m_id - %s, m_moduleName - %s\n", name.GetModuleNameString().c_str(), GetModuleName().GetModuleNameString().c_str());

	ProtoMessage<ModuleName> mnMsg(&m_handler);
	*mnMsg.mutable_ipc_name() = GetModuleName();
	mnMsg.set_ip("");
	mnMsg.set_port(0);
	mnMsg.set_access_id(m_userName);
	toMessage(mnMsg);
}

void ClientServerConnector::onMessage(const Login& msg)
{
	if(m_checker)
	{
		m_checker->Stop();
		m_checker = 0;
	}

	LoginMessage loginMsg(this, msg);
	onSignal(loginMsg);

	LoginResultMessage loginResultMsg(this);
	loginResultMsg.set_login_result(loginMsg.login_result());
	loginResultMsg.set_own_session_id(CreateGUID());
	toMessage(loginResultMsg);

	if(loginMsg.login_result() == LOGIN_FAILURE)
	{
		LOG_INFO("Login failed: m_moduleName - %s\n", GetModuleName().GetModuleNameString().c_str());
		Stop();
		return;
	}

	m_ownSessionId = loginResultMsg.own_session_id();
	SetAccessId(msg.name());
	
	IPCObjectName name(GetId(), m_ownSessionId);
	SetId(name.GetModuleNameString());
	IPCConnector::SetModuleName(IPCObjectName(GetModuleName().module_name(), m_ownSessionId));
	
	LOG_INFO("Login succesful: m_id - %s, m_moduleName - %s\n", name.GetModuleNameString().c_str(), GetModuleName().GetModuleNameString().c_str());

	ProtoMessage<ModuleName> mnMsg(&m_handler);
	*mnMsg.mutable_ipc_name() = GetModuleName();
	mnMsg.set_ip("");
	mnMsg.set_port(0);
	mnMsg.set_access_id(msg.name());
	toMessage(mnMsg);
}

void ClientServerConnector::onMessage(const PeerData& msg)
{
	PeerDataSignal pdSig(msg);
	onSignal(pdSig);
}

void ClientServerConnector::onMessage(const AvailablePearTypes& msg)
{
    AvailablePearTypesSignal aptSig(msg);
    onSignal(aptSig);
}
    
void ClientServerConnector::onMessage(const InitTunnelComplete& msg)
{
	InitTunnelCompleteMessage itcMsg(this, msg);
	onSignal(itcMsg);
}

void ClientServerConnector::onMessage(const InitTunnel& msg)
{
	IPCObjectName idName = IPCObjectName::GetIPCName(m_id);
	LOG_INFO("InitTunnel message: m_id - %s, from %s with %s\n", idName.GetModuleNameString().c_str(), msg.own_session_id().c_str(), msg.ext_session_id().c_str());
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

void ClientServerConnector::onMessage(const ModuleName& msg)
{
	m_handler.onMessage(msg);
	
	IPCObjectName idName = IPCObjectName::GetIPCName(m_id);
	if(idName.module_name() == ClientServerModule::m_clientIPCName)
	{
		IPCObjectListMessage ipcolMsg(&m_handler);
		ipcolMsg.set_access_id(GetAccessId());
		onIPCSignal(ipcolMsg);
		toMessage(ipcolMsg);

		AddIPCObjectMessage aipcoMsg(&m_handler);
		aipcoMsg.set_ip("");
		aipcoMsg.set_port(0);
		aipcoMsg.set_access_id(GetAccessId());
		*aipcoMsg.mutable_ipc_name() = idName;
		onIPCSignal(aipcoMsg);
	}
}

void ClientServerConnector::onMessage(const TryConnectTo& msg)
{
	TryConnectToMessage tctMsg(this, msg);
	IPCObjectName idName = IPCObjectName::GetIPCName(m_id);
	LOG_INFO("TryConnectTo message: m_id - %s, from %s with %s by type %d\n", idName.GetModuleNameString().c_str(), msg.own_session_id().c_str(), msg.ext_session_id().c_str(), msg.type());
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
