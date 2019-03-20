#ifndef TWAINET_MODULE_H
#define TWAINET_MODULE_H

#include "tunnel_lib/module/tunnel_module.h"

class TwainetModule : public TunnelModule
{
public:
    class WaitData
    {
    private:
        WaitData() : m_from(""){}
        WaitData(const WaitData& data) : m_from(""){}
    public:
        explicit WaitData(const std::string& messageName, const IPCObjectName& from)
        : m_messageName(messageName), m_from(from){}
        ~WaitData(){}
        
        bool operator == (const WaitData& data) const {
            return  m_messageName == data.m_messageName &&
                    m_from == data.m_from;
        }
        
        std::string m_messageName;
        Semaphore m_semaphore;
        IPCObjectName m_from;
        std::string m_data;
    };
public:
	TwainetModule(const IPCObjectName& ipcName, int ipv);
	~TwainetModule();
public:
	std::vector<IPCObjectName> GetTargetPath(const IPCObjectName& target);
    
    	bool SendSyncMsg(const IPCMessageSignal& msg, const std::string& messageName, const IPCObjectName& from, std::string& data);
protected:
	// Tunnel with other computer was not created
	virtual void OnTunnelConnectFailed(const std::string& sessionId);

	// Tunnel with other computer was created
	virtual void OnTunnelConnected(const std::string& sessionId, TunnelConnector::TypeConnection type);
	
	// Client was connected to server
	virtual void OnServerConnected();

	// Server was connected to client
	virtual void OnClientConnector(const std::string& sessionId);
	
	// connect with module failed
	virtual void OnConnectFailed(const std::string& moduleName);

	// connect with server is successful, but login is failed
	virtual void OnAuthFailed();

	// Connection with module was created.
	virtual void OnConnected(const std::string& moduleName);

	// Connection with module was destroyed.
	// Tunnel connection this is the module's connection also,
	// therefore this function will called also
	// Client or server connection this is the module's connection also,
	// therefore this function will called also
	virtual void OnFireConnector(const std::string& moduleName);

	// It has got message
	virtual void OnMessage(const std::string& messageName, const std::vector<std::string>& path, const std::string& data);
    

	// Module has not created
	virtual void ModuleCreationFailed();
	
	// Server has not created
	virtual void ServerCreationFailed();

	// Changed internal connection status
	virtual void OnInternalConnection(const std::string& moduleName, ConnectionStatus status, int port);
	
	// IPC Object list was changed
	virtual void OnIPCObjectsChanged();
    
protected:
    virtual void ManagerStop();
private:
    CriticalSection m_cs;
    std::vector<WaitData*> m_waitingList;
};

#endif/*TWAINET_MODULE_H*/
