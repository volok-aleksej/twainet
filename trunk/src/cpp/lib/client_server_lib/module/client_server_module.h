#ifndef CLIENT_MODULE_H
#define CLIENT_MODULE_H

#include "client_server_lib/connector/client_server_connector.h"
#include "ipc_lib/module/ipc_module.h"
#include "connector_lib/message/connector_messages.h"

class ClientServerModule : public IPCModule
{
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

	void StartServer(int port);
	bool IsStopServer();
	void StopServer();

	std::string GetSessionId();
protected:
	virtual void OnFireConnector(const std::string& moduleName);
	virtual bool CheckFireConnector(const std::string& moduleName);
	virtual void OnServerConnected();
	virtual void OnClientConnector(const std::string& sessionId);
protected:
	friend class Signal;
	void onAddConnector(const ConnectorMessage& msg);
	void onErrorConnect(const ConnectErrorMessage& msg);
	void onCreatedListener(const CreatedListenerMessage& msg);
	void onErrorListener(const ListenErrorMessage& msg);

	void onLogin(const LoginMessage& msg);
	void onLoginResult(const LoginResultMessage& msg);
	void onIPCMessage(const IPCProtoMessage& msg);

	//for client
	std::string m_ownSessionId;
private:
	//for client
	std::string m_ip;
	int m_port;
	bool m_isStopConnect;
	std::string m_userName;
	std::string m_password;

	//for server
	IPCListenThread* m_serverThread;
};


#endif/*CLIENT_MODULE_H*/