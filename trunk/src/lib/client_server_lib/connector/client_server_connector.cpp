#include "client_server_connector.h"
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
}

ClientServerConnector::~ClientServerConnector()
{
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
	}
}

void ClientServerConnector::Subscribe(::SignalOwner* owner)
{
	owner->addSubscriber(this, SIGNAL_FUNC(this, ClientServerConnector, IPCMessageSignal, onIPCMessage));
	owner->addSubscriber(this, SIGNAL_FUNC(this, ClientServerConnector, InitTunnelSignal, onInitTunnelSignal));
	owner->addSubscriber(this, SIGNAL_FUNC(this, ClientServerConnector, InitTunnelStartedSignal, onInitTunnelStartedSignal));
	owner->addSubscriber(this, SIGNAL_FUNC(this, ClientServerConnector, TryConnectToSignal, onTryConnectToSignal));
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
	if(m_id == ClientServerModule::m_serverIPCName)
	{
		ProtoMessage<Login, ClientServerConnector> loginMsg(this);
		loginMsg.set_name(m_userName);
		loginMsg.set_pasword(m_password);
		toMessage(loginMsg);
	}
}

void ClientServerConnector::onIPCMessage(const IPCProtoMessage& msg)
{
	IPCObjectName path(msg.ipc_path(0));
	if(path.GetModuleNameString() == m_id)
	{
		ClientSendMessage sendMsg(msg);
		toMessage(sendMsg);
	}
}

void ClientServerConnector::onIPCMessage(const IPCMessageSignal& msg)
{
	IPCObjectName ipcName("");
	if(msg.ipc_path_size() != 0)
	{
		ipcName = const_cast<IPCMessageSignal&>(msg).ipc_path(0);
	}

	if (msg.ipc_path_size() == 0 ||
		ipcName == IPCObjectName::GetIPCName(GetId()))
	{
		ClientSendMessage sendMsg(msg);
		toMessage(sendMsg);
	}
}

void ClientServerConnector::onInitTunnelSignal(const InitTunnelSignal& msg)
{
	if((m_id == ClientServerModule::m_clientIPCName || !msg.has_type())
		&& msg.own_session_id() == m_ownSessionId)
	{
		InitTunnelMessage itMsg(this, msg);
		toMessage(itMsg);
	}
}


void ClientServerConnector::onInitTunnelStartedSignal(const InitTunnelStartedSignal& msg)
{
	if(m_id == ClientServerModule::m_clientIPCName
		&& msg.own_session_id() == m_ownSessionId)
	{
		InitTunnelStartedMessage itrMsg(this, msg);
		toMessage(itrMsg);
	}
}

void ClientServerConnector::onTryConnectToMessage(const TryConnectToMessage& msg)
{
	if(m_id == ClientServerModule::m_clientIPCName
		&& msg.own_session_id() == m_ownSessionId)
	{
		toMessage(msg);
	}
}

void ClientServerConnector::onTryConnectToSignal(const TryConnectToSignal& msg)
{
	if(((m_id == ClientServerModule::m_serverIPCName
		&& msg.type() == TUNNEL_LOCAL)
		|| (m_id == ClientServerModule::m_clientIPCName
		&& msg.type() == TUNNEL_EXTERNAL))
		&& msg.own_session_id() == m_ownSessionId)
	{
		TryConnectToMessage tctMsg(this, msg);
		toMessage(tctMsg);
	}	
}

void ClientServerConnector::onMessage(const LoginResult& msg)
{
	printf("LoginResult: %s\n", msg.own_session_id().c_str());
	m_ownSessionId = msg.own_session_id();

	LoginResultMessage lrMsg(this, msg);
	onSignal(lrMsg);
}

void ClientServerConnector::onMessage(const Login& msg)
{
	printf("Login: %s\n", msg.name().c_str());

	ProtoMessage<LoginResult, ClientServerConnector> loginResultMsg(this);
	loginResultMsg.set_login_result(LOGIN_SUCCESS);
	loginResultMsg.set_own_session_id(CreateGUID());
	toMessage(loginResultMsg);

	m_ownSessionId = loginResultMsg.own_session_id();

	printf("sessionId: %s\n", m_ownSessionId.c_str());
}

void ClientServerConnector::onMessage(const InitTunnel& msg)
{
	if(m_id == ClientServerModule::m_clientIPCName)
	{
		InitTunnelSignal itMsg(msg);
		onSignal(itMsg);
	}
	else if(m_id == ClientServerModule::m_serverIPCName)
	{
		InitTunnelMessage itMsg(this, msg);
		onSignal(itMsg);
	}
}

void ClientServerConnector::onMessage(const TryConnectTo& msg)
{
	TryConnectToMessage tctMsg(this, msg);
	if(m_id == ClientServerModule::m_clientIPCName)
	{
		tctMsg.set_own_session_id(msg.ext_session_id());
		tctMsg.set_ext_session_id(msg.own_session_id());
		onIPCSignal(tctMsg);
	}
	else if(m_id == ClientServerModule::m_serverIPCName)
	{
		onSignal(tctMsg);
	}
}

void ClientServerConnector::onMessage(const InitTunnelStarted& msg)
{
	InitTunnelStartedMessage itrMsg(this, msg);
	onSignal(itrMsg);
}