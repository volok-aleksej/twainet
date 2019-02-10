#ifndef MODULE_H
#define MODULE_H

#include <map>
#include <stdio.h>
#include <string.h>
#include "twainet.h"
#include "client_module_name.h"
#include "deamon_message.h"
#include "object_manager.h"

#ifndef LIGTH_VERSION
#pragma warning(disable:4244 4267)
#include "messages/deamon.pb.h"
using namespace deamon;
#pragma warning(default:4244 4267)

class Module;

typedef UserMessage<LocalServerAttributes, Module> LocalServerAttributesMessage;
typedef UserMessage<ClientName, Module> ClientNameMessage;
typedef UserMessage<ClientNameList, Module> ClientNameListMessage;
typedef UserMessage<SetConfig, Module> SetConfigMessage;
typedef UserMessage<GetConfig, Module> GetConfigMessage;
typedef UserMessage<InstallPlugin, Module> InstallPluginMessage;
#endif/*LIGTH_VERSION*/

class IModule
{
public:
      virtual ~IModule(){}
      virtual void Create() = 0;
      virtual void AddMessage(DataMessage* msg) = 0;
      virtual bool toMessage(const DataMessage& msg, const Twainet::ModuleName& path) = 0;
      virtual bool toMessage(const DataMessage& msg, const Twainet::ModuleName& path, DataMessage& resp) = 0;
      virtual const Twainet::Module GetModule() = 0;
      virtual void Free() = 0;
      virtual void OnServerConnected(const char* sessionId) = 0;
      virtual void OnServerDisconnected() = 0;
      virtual void OnClientConnected(const char* sessionId) = 0;
      virtual void OnClientDisconnected(const char* sessionId) = 0;
      virtual void OnClientConnectionFailed() = 0;
      virtual void OnClientAuthFailed() = 0;
      virtual void OnModuleConnected(const Twainet::ModuleName& moduleId) = 0;
      virtual void OnModuleDisconnected(const Twainet::ModuleName& moduleId) = 0;
      virtual void OnModuleConnectionFailed(const Twainet::ModuleName& moduleId) = 0;
      virtual void OnMessageRecv(const Twainet::Message& msg) = 0;
      virtual void OnModuleListChanged() = 0;
      virtual void OnTunnelCreationFailed(const char* sessionId) = 0;
      virtual void OnTunnelConnected(const char* sessionId, Twainet::TypeConnection type) = 0;
      virtual void OnTunnelDisconnected(const char* sessionId) = 0;
      virtual void OnInternalConnectionStatusChanged(const char* moduleName, Twainet::InternalConnectionStatus status, int port) = 0;
};

class Module : public IModule
{
public:
	Module(const std::string& moduleName, Twainet::IPVersion ipv = Twainet::IPV4, bool isCoord = false)
	  : m_module(0), m_moduleName(moduleName), m_isCoord(isCoord), m_ipv(ipv)
	{
#ifndef LIGTH_VERSION
		AddMessage(new LocalServerAttributesMessage(this));
		AddMessage(new ClientNameListMessage(this));
		AddMessage(new ClientNameMessage(this));
		AddMessage(new SetConfigMessage(this));
		AddMessage(new GetConfigMessage(this));
		AddMessage(new InstallPluginMessage(this));
#endif/*LIGTH_VERSION*/
	}
	virtual ~Module()
	{
		Twainet::DeleteModule(m_module);
	}

    virtual void Create()
    {
        m_module = Twainet::CreateModule(m_moduleName.c_str(), m_ipv, m_isCoord);
    }

public:
	virtual void OnTunnelCreationFailed(const char* sessionId)
	{
	}

	virtual void OnServerConnected(const char* sessionId)
	{
#ifndef LIGTH_VERSION
		Twainet::ModuleName moduleName = {0};
		strcpy(moduleName.m_name, Twainet::ServerModuleName);
		strcpy(moduleName.m_host, sessionId);
		ClientNameMessage cnMsg(this);
		cnMsg.set_ipc_name(Twainet::GetModuleName(m_module).m_name);
		cnMsg.set_host_name(sessionId);
		toMessage(cnMsg, moduleName);
#endif/*LIGTH_VERSION*/
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
//        printf("message %s recv\n", msg.m_typeMessage);
		onData(msg.m_typeMessage, msg.m_target, (char*)msg.m_data, msg.m_dataLen);
	}

	virtual void OnInternalConnectionStatusChanged(const char* moduleName,
						       Twainet::InternalConnectionStatus status, int port)
	{
	}

	virtual void OnModuleListChanged()
	{
#ifndef LIGTH_VERSION
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
#endif/*LIGTH_VERSION*/
	}

protected:
	virtual void AddMessage(DataMessage* msg)
	{
		std::map<std::string, DataMessage*>::iterator it = m_messages.find(msg->GetName());
		if(it != m_messages.end()) {
			it->second = msg;
		} else {
			m_messages.insert(std::make_pair(msg->GetName(), msg));
		}
	}

	virtual bool toMessage(const DataMessage& msg, const Twainet::ModuleName& path)
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

		delete[] data;
		return ret;
	}

	virtual bool toMessage(const DataMessage& msg, const Twainet::ModuleName& path, DataMessage& resp)
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

            std::string respTypeMessage = resp.GetName();
			Twainet::Message respmessage = {0};
			memcpy((void*)&respmessage.m_target, (void*)&path, sizeof(path));
			respmessage.m_typeMessage = respTypeMessage.c_str();

			if(Twainet::SendSyncMessage(m_module, message, respmessage)) {
				ret = resp.serialize(const_cast<char*>(respmessage.m_data), respmessage.m_dataLen);
				Twainet::FreeMessage(respmessage);
			} else {
				ret = false;
			}
		}

		delete[] data;
		return ret;
	}

	virtual const Twainet::Module GetModule()
	{
		return m_module;
	}

	virtual void Free()
	{
		delete this;
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

protected:
#ifndef LIGTH_VERSION
	virtual void OnInstallPluginRequest(const InstallPlugin& msg)
	{
	}

	/*******************************************************************************************/
	/*                                      server functions                                   */
	/*******************************************************************************************/
	virtual void OnConfigChanged(const SetConfig& msg)
	{
	}

	/*******************************************************************************************/
	/*                                      client functions                                   */
	/*******************************************************************************************/
	virtual void OnConfig(const SetConfig& msg)
	{
	}

private:
	template<class TMessage, class THandler> friend class UserMessage;

	/*******************************************************************************************/
	/*                                    messages for all                                     */
	/*******************************************************************************************/
	void onMessage(const SetConfig& msg, const Twainet::ModuleName& path)
	{
		if(strcmp(path.m_name, Twainet::ClientModuleName) != 0)
		{
			// from all besides the client
			OnConfig(msg);
		}
		else
		{
			// from client to server
			OnConfigChanged(msg);
		}

		m_config = msg;
	}

	void onMessage(const GetConfig& msg, const Twainet::ModuleName& path)
	{
		SetConfigMessage scMsg(this, m_config);
		toMessage(scMsg, path);
	}

	void onMessage(const InstallPlugin& msg, const Twainet::ModuleName& path)
	{
		OnInstallPluginRequest(msg);
	}

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
			   strcmp(names[i].m_name, Twainet::ClientModuleName) != 0)
				continue;

			toMessage(cnlMsg, names[i]);
		}
		delete names;
	}
#endif/*LIGTH_VERSION*/

protected:
#ifndef LIGTH_VERSION
	ObjectManager<ClientModuleName> m_clientsNameOnServer;
	ObjectManager<ClientModuleName> m_clientsNameOnClient;
	SetConfig m_config;
#endif/*LIGTH_VERSION*/
	Twainet::Module m_module;
private:
	std::map<std::string, DataMessage*> m_messages;
	std::string m_moduleName;
	bool m_isCoord;
	Twainet::IPVersion m_ipv;
};

#endif/*MODULE_H*/
