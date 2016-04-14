#include "deamon_module.h"
#include "common/common_func.h"
#include "common/dir.h"
#include "common/file.h"
#include "utils/utils.h"
#include "../application/config.h"

#define MAX_BUFFER_LEN 1024

DeamonModule::DeamonModule()
: Module(COORDINATOR_NAME, Twainet::IPV4, true)
{
	ReadConfig();
	
	m_config.set_local_port(Config::GetInstance().GetLocalServerPort());
	m_config.set_trusted_file_name(Config::GetInstance().GetTrustedFileName());
	
	strcpy(m_userPassword.m_user, CreateGUID().c_str());
	strcpy(m_userPassword.m_pass, CreateGUID().c_str());
	Twainet::SetUsersList(m_module, &m_userPassword, 1);
	int port = Config::GetInstance().GetLocalServerPort();
	Twainet::CreateServer(m_module, port, Twainet::IPV4, true);
	
	AddMessage(new TestMessage(this));
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
			msg.set_port(Config::GetInstance().GetLocalServerPort());
			msg.set_username(m_userPassword.m_user);
			msg.set_password(m_userPassword.m_pass);
			toMessage(msg, moduleName);
			break;
		}
	}
}

void DeamonModule::OnConfigChanged(const SetConfig& msg)
{
	Config::GetInstance().SetLocalServerPort(msg.local_port());
	Config::GetInstance().SetTrustedFileName(msg.trusted_file_name());
}

void DeamonModule::onMessage(const Test& test, Twainet::ModuleName path)
{
}

void DeamonModule::ReadConfig()
{
	std::string fileName = Config::GetInstance().GetTrustedFileName();
#ifdef WIN32
	std::string configPath = Dir::GetConfigDir() + "\\" + fileName;
#else
	std::string configPath = Dir::GetConfigDir() + "/" + fileName;
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
