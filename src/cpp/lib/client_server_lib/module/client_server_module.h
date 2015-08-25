#ifndef CLIENT_MODULE_H
#define CLIENT_MODULE_H

#include "client_server_lib/connector/client_server_connector.h"
#include "ipc_lib/module/ipc_module.h"
#include "connector_lib/message/connector_messages.h"
#include "client_server_lib/message/client_server_messages.h"

class ClientServerModule : public IPCModule
{
	class UserPassword
	{
public:
		UserPassword(std::string userName = "", std::string password = "")
			: m_userName(userName), m_password(password){}
		
		void operator = (const UserPassword& userPass)
		{
			m_userName = userPass.m_userName;
			m_password = userPass.m_password;
		}

		bool operator == (const UserPassword& userPass)
		{
			return m_userName == userPass.m_userName;
		}
		
		bool operator != (const UserPassword& userPass)
		{
			return m_userName != userPass.m_userName;
		}

		bool operator < (const UserPassword& userPass) const
		{
			return m_userName < userPass.m_userName;
		}

		std::string m_userName;
		std::string m_password;
	};
public:
	static const std::string m_serverIPCName;
	static const std::string m_clientIPCName;

public:
	ClientServerModule(const IPCObjectName& ipcName, ConnectorFactory* factory);
	virtual ~ClientServerModule();
public:
	void Connect(const std::string& ip, int port);
	void Disconnect();
	void SetUserName(const std::string& userName);
	void SetPassword(const std::string& password);
	void AddUser(const std::string& userName, const std::string& password);
	void RemoveUser(const std::string& userName, const std::string& password);
	void ClearUsers();
	void UseStandartConnections();
	void UseProxy(const std::string& ip, int port);
	void SetProxyUserName(const std::string& userName);
	void SetProxyPassword(const std::string& password);

	void StartServer(int port);
	bool IsStopServer();
	void StopServer();

	const std::string& GetSessionId();
protected:
	virtual void OnFireConnector(const std::string& moduleName);
	virtual bool CheckFireConnector(const std::string& moduleName);
	virtual void OnServerConnected();
	virtual void OnClientConnector(const std::string& sessionId);
	virtual void ServerCreationFailed();
	virtual void OnAuthFailed();
	virtual void FillIPCObjectList(IPCObjectListMessage& msg);
protected:
	friend class Signal;
	void onAddConnector(const ConnectorMessage& msg);
	void onErrorConnect(const ConnectErrorMessage& msg);
	void onCreatedListener(const CreatedListenerMessage& msg);
	void onErrorListener(const ListenErrorMessage& msg);
	void onConnected(const ClientServerConnectedMessage& msg);

	void onLogin(const LoginMessage& msg);
	void onLoginResult(const LoginResultMessage& msg);

	//for client
	std::string m_ownSessionId;
	std::string m_ip;
	int m_port;

	//for client with proxy
	bool m_isUseProxy;
	std::string m_proxyIp;
	int m_proxyPort;
	UserPassword m_proxyUserPassword;
private:
	//for client
	bool m_isStopConnect;
	UserPassword m_userPassword;


	//for server
	BaseListenThread* m_serverThread;
	ObjectManager<UserPassword> m_userPasswords;
};


#endif/*CLIENT_MODULE_H*/