#include "deamon_module.h"
#include "common/common.h"
#include "common/common_func.h"
#include "common/dir.h"
#include "common/file.h"
#include "utils/utils.h"

#define MAX_BUFFER_LEN 1024

/*******************************************************************************/
/*                             ClientModuleName                                */
/*******************************************************************************/
DeamonModule::ClientModuleName::ClientModuleName()
{
}

DeamonModule::ClientModuleName::ClientModuleName(const DeamonModule::ClientModuleName& name)
{
	operator = (name);
}

DeamonModule::ClientModuleName::ClientModuleName(const std::string& moduleName, const std::string& hostClient)
: m_moduleName(moduleName), m_hostClient(hostClient)
{
}

DeamonModule::ClientModuleName::~ClientModuleName()
{
}
		
void DeamonModule::ClientModuleName::operator = (const DeamonModule::ClientModuleName& name)
{
	m_moduleName = name.m_moduleName;
	m_hostClient = name.m_hostClient;
}

bool DeamonModule::ClientModuleName::operator == (const DeamonModule::ClientModuleName& name) const
{
	return m_hostClient == name.m_hostClient;
}

bool DeamonModule::ClientModuleName::operator != (const DeamonModule::ClientModuleName& name) const
{
	return !operator == (name);
}

bool DeamonModule::ClientModuleName::operator < (const DeamonModule::ClientModuleName& name) const
{
	return m_hostClient < name.m_hostClient;
}

/*******************************************************************************/
/*                             DeamonModule                                    */
/*******************************************************************************/
DeamonModule::DeamonModule(const Twainet::Module& module)
: Module(module)
{
	ReadConfig();
	
	strcpy(m_userPassword.m_user, CreateGUID().c_str());
	strcpy(m_userPassword.m_pass, CreateGUID().c_str());
	Twainet::SetUsersList(module, &m_userPassword, 1);
	Twainet::CreateServer(module, g_localServerPort, Twainet::IPV4, true);
}

DeamonModule::~DeamonModule()
{
}


void DeamonModule::OnModuleConnected(const Twainet::ModuleName& moduleName)
{
	if(strlen(moduleName.m_host) != 0)
		return;
	
	int sizeName = 0;
	Twainet::GetModuleNameString(moduleName, 0, sizeName);
	std::string strModuleName(sizeName, 0);
	Twainet::GetModuleNameString(moduleName, (char*)strModuleName.c_str(), sizeName);
	
	for(std::vector<std::string>::iterator it = m_trustedModules.begin();
	    it != m_trustedModules.end(); it++)
	{
		if(*it == strModuleName)
		{
			LocalServerAttributesMessage msg(this);
			msg.set_port(g_localServerPort);
			msg.set_username(m_userPassword.m_user);
			msg.set_password(m_userPassword.m_pass);
			toMessage(msg, moduleName);
			break;
		}
	}
}

void DeamonModule::OnMessageRecv(const Twainet::Message& message)
{
	onData(message.m_typeMessage, message.m_target, (char*)message.m_data, message.m_dataLen);
}

void DeamonModule::OnModuleListChanged()
{
	Twainet::ModuleName* names = 0;
	int sizeNames = 0;
	sizeNames = Twainet::GetExistingModules(GetModule(), names, sizeNames);
	names = new Twainet::ModuleName[sizeNames];
	Twainet::GetExistingModules(GetModule(), names, sizeNames);
	std::vector<ClientModuleName> clients = m_clientsName.GetObjectList();
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
			m_clientsName.RemoveObject(*it);
		}
	}
	delete names;
}

void DeamonModule::onMessage(const ClientName& msg, const Twainet::ModuleName& path)
{
	ClientModuleName clientName(msg.ipc_name(), msg.host_name());
	if(!m_clientsName.AddObject(clientName))
	{
		m_clientsName.UpdateObject(clientName);
	}
	
	ClientNameListMessage cnlMsg(this);
	std::vector<ClientModuleName> clients = m_clientsName.GetObjectList();
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
	sizeNames = Twainet::GetExistingModules(GetModule(), names, sizeNames);
	names = new Twainet::ModuleName[sizeNames];
	Twainet::GetExistingModules(GetModule(), names, sizeNames);
	for(int i = 0; i < sizeNames; i++)
	{
		if(strlen(names[i].m_host) == 0)
			continue;
		
		toMessage(cnlMsg, names[i]);
	}
	delete names;
}
	
void DeamonModule::ReadConfig()
{
#ifdef WIN32
	std::string configPath = Dir::GetConfigDir() + "\\twainet.trusted";
#else
	std::string configPath = Dir::GetConfigDir() + "/twainet.trusted";
#endif/*WIN32*/
	File configFile(configPath);
	int filesize = configFile.GetFileSize(), filepos = 0;
	char* data = new char[MAX_BUFFER_LEN];
	char* dataPos = data;
	while(filepos < filesize)
	{
		unsigned int size = MAX_BUFFER_LEN - (unsigned int)(dataPos - data) ;
		memset(dataPos, 0, size);
		if(!configFile.Read(dataPos, &size))
		{
			break;
		}
		filepos += size;
		
		std::vector<std::string> lines = CommonUtils::DelimitString(data, "\n");
		int pos = 0;
		for(int i = 0; i < (int)lines.size() - 1; i++)
		{
			pos += lines[i].size() + 1;
			std::string line = lines[i];
			if(line[line.size() - 1] == '\r')
				line.erase(line.size() - 1, 1);
			if(line[0] == '#' || line.empty())
				continue;
			
			m_trustedModules.push_back(line);
		}
		
		int templen = filepos - pos;
		char* tempData = new char[templen];
		memcpy(tempData, data + pos, templen);
		memcpy(data, tempData, templen);
		delete tempData;
		dataPos = data + templen;
	}
	
	if(dataPos - data)
	{
		m_trustedModules.push_back(std::string(data, dataPos - data));
	}
	delete data;
}
