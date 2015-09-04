#ifndef MODULE_H
#define MODULE_H

#include <map>
#include "twainet/application/twainet.h"
#include "connector_lib/handler/data_message.h"
#include "deamon_message.h"

class Module
{
public:
	Module(const Twainet::Module& module)
	  : m_module(module){}
	virtual ~Module()
	{
		Twainet::DeleteModule(m_module);
	}
	
	virtual void OnTunnelCreationFailed(const char* sessionId){}
	virtual void OnServerConnected(const char* sessionId){}
	virtual void OnClientConnected(const char* sessionId){}
	virtual void OnClientDisconnected(const char* sessionId){}
	virtual void OnClientConnectionFailed(){}
	virtual void OnClientAuthFailed(){}
	virtual void OnServerDisconnected(){}
	virtual void OnModuleConnected(const Twainet::ModuleName& moduleId){}
	virtual void OnModuleDisconnected(const Twainet::ModuleName& moduleId){}
	virtual void OnModuleConnectionFailed(const Twainet::ModuleName& moduleId){}
	virtual void OnTunnelConnected(const char* sessionId, Twainet::TypeConnection type){}
	virtual void OnTunnelDisconnected(const char* sessionId){}
	virtual void OnMessageRecv(const Twainet::Message& msg){}
	virtual void OnInternalConnectionStatusChanged(const char* moduleName, const char* id,
						       Twainet::InternalConnectionStatus status, int port){}

protected:
	void AddMessage(DataMessage* msg)
	{
		m_messages[msg->GetName()] = msg;
	}
	
	bool onData(const std::string& type, const std::vector<Twainet::ModuleName>& path, char* data, int len)
	{
		std::map<std::string, DataMessage*>::iterator it = m_messages.find(type);
		if (it != m_messages.end())
		{
			if (len >= 0)
			{
				it->second->serialize(data, len);
				MessageAttr* attr = dynamic_cast<MessageAttr*>(it->second);
				if(attr)
				{
				    attr->SetPath(path);
				    it->second->onMessage();
				    return true;
				}
			}
		}
		return false;
	}
private:
	Twainet::Module m_module;
	std::map<std::string, DataMessage*> m_messages;
};

#endif/*MODULE_H*/