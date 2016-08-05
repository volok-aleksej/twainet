#include "configs.h"
#include <string.h>

TrustedConfig::TrustedConfig(const std::string& filePath)
: FileConfig(filePath)
{
    m_trustedModules = Read();
}

TrustedConfig::~TrustedConfig()
{
    Write("Module names that will get local server credentials", m_trustedModules);
}
    
std::vector<std::string> TrustedConfig::getTrustedModules()
{
    return m_trustedModules;
}

void TrustedConfig::setTrustedModules(const std::vector<std::string>& modules)
{
    m_trustedModules = modules;
}

PluginsConfig::PluginsConfig(const std::string& filePath)
: FileConfig(filePath)
{
    m_pluginsPaths = Read();
}

PluginsConfig::~PluginsConfig()
{
    Write("Plugin pathes", m_pluginsPaths);
}
    
std::vector<std::string> PluginsConfig::getPluginsPaths()
{
    return m_pluginsPaths;
}

void PluginsConfig::setPluginsPath(const std::vector<std::string>& paths)
{
    m_pluginsPaths = paths;
}