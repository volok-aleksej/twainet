#include "deamon_module.h"
#include "common/common_func.h"
#include "common/dir.h"
#include "../application/configs.h"

DeamonModule::DeamonModule()
: Module(COORDINATOR_NAME, Twainet::IPV4, true)
{
	ReadConfig();
	
	m_config.set_local_port(Config::GetInstance().GetLocalServerPort());
    for(std::vector<std::string>::iterator it = m_trustedModules.begin();
        it != m_trustedModules.end(); it++)
    {
        m_config.add_trusted_modules(*it);
    }
    
    for(std::vector<std::string>::iterator it = m_pluginPaths.begin();
        it != m_pluginPaths.end(); it++)
    {
        Plugin* plugin = m_config.add_plugins();
        plugin->set_path(*it);
    }
	
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
    m_trustedModules.clear();
    for(int i = 0; i < msg.trusted_modules_size(); i++)
    {
        m_trustedModules.push_back(msg.trusted_modules(i));
    }
    
    m_pluginPaths.clear();
    for(int i = 0; i < msg.plugins_size(); i++)
    {
        m_pluginPaths.push_back(msg.plugins(i).path());
    }
    
    TrustedConfig trustedConfig(Config::GetInstance().GetTrustedFileName());
	trustedConfig.setTrustedModules(m_trustedModules);
    
    PluginsConfig pluginConfig(Config::GetInstance().GetPluginsFileName());
    pluginConfig.setPluginsPath(m_pluginPaths);
}

void DeamonModule::onMessage(const Test& test, Twainet::ModuleName path)
{
}

void DeamonModule::ReadConfig()
{
	std::string fileName, configPath;
    
    fileName = Config::GetInstance().GetTrustedFileName();
#ifdef WIN32
	configPath = Dir::GetConfigDir() + "\\" + fileName;
#else
	configPath = Dir::GetConfigDir() + "/" + fileName;
#endif/*WIN32*/
    TrustedConfig trustedConfig(configPath);
    m_trustedModules = trustedConfig.getTrustedModules();
    
    fileName = Config::GetInstance().GetPluginsFileName();
#ifdef WIN32
    configPath = Dir::GetConfigDir() + "\\" + fileName;
#else
    configPath = Dir::GetConfigDir() + "/" + fileName;
#endif/*WIN32*/
    PluginsConfig pluginsConfig(configPath);
    m_pluginPaths = pluginsConfig.getPluginsPaths();
}
