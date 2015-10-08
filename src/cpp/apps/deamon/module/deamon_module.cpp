#include "deamon_module.h"
#include "common/common.h"
#include "common/common_func.h"
#include "common/dir.h"
#include "common/file.h"
#include "utils/utils.h"

#define MAX_BUFFER_LEN 1024

DeamonModule::DeamonModule(const Twainet::Module& module)
: Module(module)
{
	ReadConfig();
	
	strcpy(m_userPassword.m_user, CreateGUID().c_str());
	strcpy(m_userPassword.m_pass, CreateGUID().c_str());
	Twainet::SetUsersList(module, &m_userPassword, 1);
	Twainet::CreateServer(module, g_localServerPort, true);
}

DeamonModule::~DeamonModule()
{
}


void DeamonModule::OnModuleConnected(const Twainet::ModuleName& moduleName)
{
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
			toMessage(msg, &moduleName, 1);
			break;
		}
	}
}

void DeamonModule::OnMessageRecv(const Twainet::Message& message)
{
	std::vector<Twainet::ModuleName> path;
	for(int i = 0; i < message.m_pathLen; i++)
	{
		path.push_back(message.m_path[i]);
	}
	onData(message.m_typeMessage, path, (char*)message.m_data, message.m_dataLen);
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