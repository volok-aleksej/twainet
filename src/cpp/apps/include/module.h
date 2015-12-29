#ifndef MODULE_H
#define MODULE_H

#include <map>
#include <string.h>
#include "twainet/application/twainet.h"
#include "connector_lib/handler/data_message.h"
#include "thread_lib/common/object_manager.h"
#include "client_module_name.h"
#include "deamon_message.h"

#pragma warning(disable:4244 4267)
#include "../messages/deamon.pb.h"
using namespace deamon;
#pragma warning(default:4244 4267)

class Module;

typedef DeamonMessage<LocalServerAttributes, Module> LocalServerAttributesMessage;
typedef DeamonMessage<ClientName, Module> ClientNameMessage;
typedef DeamonMessage<ClientNameList, Module> ClientNameListMessage;

class Module
{
public:
	Module(const std::string& moduleName, Twainet::IPVersion ipv, bool isCoord)
	  : m_module(0)
	{
		m_module = Twainet::CreateModule(moduleName.c_str(), ipv, isCoord);
		
		AddMessage(new LocalServerAttributesMessage(this));
		AddMessage(new ClientNameListMessage(this));
		AddMessage(new ClientNameMessage(this));
	}
	virtual ~Module()
	{
		Twainet::DeleteModule(m_module);
	}
	
	virtual void OnTunnelCreationFailed(const char* sessionId)
	{
	}
	
	virtual void OnServerConnected(const char* sessionId)
	{
		Twainet::ModuleName moduleName = {0};
		strcpy(moduleName.m_name, Twainet::ServerModuleName);
		strcpy(moduleName.m_host, sessionId);
		ClientNameMessage cnMsg(this);
		cnMsg.set_ipc_name(Twainet::GetModuleName(m_module).m_name);
		cnMsg.set_host_name(sessionId);
		toMessage(cnMsg, moduleName);
	}

	virtual void OnClientConnected(const char* sessionId)
	{
	}
	
	virtual void OnClientDisconnected(const char* sessionId)
	{
	}
	
	virtual void OnClientConnectionFailed()
	{
	}
	
	virtual void OnClientAuthFailed()
	{
	}
	
	virtual void OnServerDisconnected()
	{
	}
	
	virtual void OnModuleConnected(const Twainet::ModuleName& moduleId)
	{
	}
	
	virtual void OnModuleDisconnected(const Twainet::ModuleName& moduleId)
	{
	}
	
	virtual void OnModuleConnectionFailed(const Twainet::ModuleName& moduleId)
	{
	}
	
	virtual void OnTunnelConnected(const char* sessionId, Twainet::TypeConnection type)
	{
	}
	
	virtual void OnTunnelDisconnected(const char* sessionId)
	{
	}
	
	virtual void OnMessageRecv(const Twainet::Message& msg)
	{
		onData(msg.m_typeMessage, msg.m_target, (char*)msg.m_data, msg.m_dataLen);
	}
	
	virtual void OnInternalConnectionStatusChanged(const char* moduleName,
						       Twainet::InternalConnectionStatus status, int port)
	{
	}
						       
	virtual void OnModuleListChanged()
	{
		Twainet::ModuleName* names = 0;
		int sizeNames = 0;
		Twainet::GetExistingModules(GetModule(), names, sizeNames);
		names = new Twainet::ModuleName[sizeNames];
		sizeNames = Twainet::GetExistingModules(GetModule(), names, sizeNames);
		
		//for server
		std::vector<ClientModuleName> clients = m_clientsNameOnServer.GetObjectList();
		for(std::vector<ClientModuleName>::iterator it = clients.begin();
		  it != clients.end(); it++)
		{
			bool bFind = false;
			for(int i = 0; i < sizeNames; i++)
			{
				if(it->m_hostClient == names[i].m_host)
				{
					bFind = true;
				}
			}
			
			if(!bFind)
			{
				m_clientsNameOnServer.RemoveObject(*it);
			}
		}
		
		//for client
		clients = m_clientsNameOnClient.GetObjectList();
		for(std::vector<ClientModuleName>::iterator it = clients.begin();
		  it != clients.end(); it++)
		{
			bool bFind = false;
			for(int i = 0; i < sizeNames; i++)
			{
				if(it->m_hostClient == names[i].m_host)
				{
					bFind = true;
				}
			}
			
			if(!bFind)
			{
				m_clientsNameOnClient.RemoveObject(*it);
			}
		}
		
		delete names;
	}

	const Twainet::Module GetModule()
	{
		return m_module;
	}
protected:
	void AddMessage(DataMessage* msg)
	{
		m_messages[msg->GetName()] = msg;
	}
	
	bool onData(const std::string& type, const Twainet::ModuleName& path, char* data, int len)
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
	
	bool toMessage(const DataMessage& msg, const Twainet::ModuleName path)
	{
		bool ret = false;
		char* data = 0;
		int datalen = 0;
		const_cast<DataMessage&>(msg).deserialize(data, datalen);
		data = new char[datalen];
		if(const_cast<DataMessage&>(msg).deserialize(data, datalen))
		{
		      std::string typeMessage = msg.GetName();
		      Twainet::Message message = {0};
		      message.m_data = data;
		      message.m_dataLen = datalen;
		      memcpy((void*)&message.m_target, (void*)&path, sizeof(path));
		      message.m_typeMessage = typeMessage.c_str();
		      Twainet::SendMessage(m_module, message);
		      ret = true;
		}
		
		delete data;
		return ret;
	}
	
protected:
	template<class TMessage, class THandler> friend class DeamonMessage;

	/*******************************************************************************************/
	/*                                      server messages                                    */
	/*******************************************************************************************/
	void onMessage(const LocalServerAttributes& msg, const Twainet::ModuleName& path)
	{
		Twainet::UserPassword userpwd = {0};
		strcpy(userpwd.m_user, msg.username().c_str());
		strcpy(userpwd.m_pass, msg.password().c_str());
		Twainet::ConnectToServer(GetModule(), "localhost", msg.port(), userpwd);
	}
	
	void onMessage(const ClientNameList& msg, const Twainet::ModuleName& path)
	{
		for(int i = 0; i < msg.name_list_size(); i++)
		{
			ClientModuleName cmname(msg.name_list(i).ipc_name(), msg.name_list(i).host_name());
			m_clientsNameOnClient.AddObject(cmname);
		}
	}
	
	/*******************************************************************************************/
	/*                                      client messages                                    */
	/*******************************************************************************************/
	void onMessage(const ClientName& msg, const Twainet::ModuleName& path)
	{
		ClientModuleName clientName(msg.ipc_name(), msg.host_name());
		if(!m_clientsNameOnServer.AddObject(clientName))
		{
			m_clientsNameOnServer.UpdateObject(clientName);
		}
		
		ClientNameListMessage cnlMsg(this);
		std::vector<ClientModuleName> clients = m_clientsNameOnServer.GetObjectList();
		for(std::vector<ClientModuleName>::iterator it = clients.begin();
		    it != clients.end(); it++)
		{
			ClientName name;
			name.set_ipc_name(it->m_moduleName);
			name.set_host_name(it->m_hostClient);
			*cnlMsg.add_name_list() = name;
		}
		
		Twainet::ModuleName* names = 0;
		int sizeNames = 0;
		Twainet::GetExistingModules(GetModule(), names, sizeNames);
		names = new Twainet::ModuleName[sizeNames];
		Twainet::GetExistingModules(GetModule(), names, sizeNames);
		for(int i = 0; i < sizeNames; i++)
		{
			if(strlen(names[i].m_host) == 0 ||
			   strcmp(names[i].m_name, Twainet::ServerModuleName) == 0)
				continue;
			
			toMessage(cnlMsg, names[i]);
		}
		delete names;
	}
	
protected:
	ObjectManager<ClientModuleName> m_clientsNameOnServer;
	ObjectManager<ClientModuleName> m_clientsNameOnClient;
	Twainet::Module m_module;
private:
	std::map<std::string, DataMessage*> m_messages;
};

#endif/*MODULE_H*/