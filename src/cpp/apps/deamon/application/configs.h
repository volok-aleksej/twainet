#ifndef CONFIGS_H
#define CONFIGS_H

#include "config.h"

class TrustedConfig : public FileConfig
{
public:
    TrustedConfig(const std::string& filePath);
    ~TrustedConfig();
    
    std::vector<std::string> getTrustedModules();
    void setTrustedModules(const std::vector<std::string>& modules);
private:
    std::vector<std::string> m_trustedModules;
};

class PluginsConfig : public FileConfig
{
public:
    PluginsConfig(const std::string& filePath);
    ~PluginsConfig();
    
    std::vector<std::string> getPluginsPaths();
    void setPluginsPath(const std::vector<std::string>& paths);
private:
    std::vector<std::string> m_pluginsPaths;
};

#endif/*CONFIGS_H*/