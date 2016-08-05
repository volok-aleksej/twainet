#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include "common/ini_file.h"
#include "include/singleton.h"

class Config : public Singleton<Config>
{
private:
	friend class Singleton<Config>;
	Config();
	virtual ~Config();
	
public:	
	std::string GetTrustedFileName();
    std::string GetPluginsFileName();
	int GetLocalServerPort();
	void SetLocalServerPort(int port);
private:
	IniFile m_file;
};

class FileConfig
{
public:
    FileConfig(const std::string& filePath);
    virtual ~FileConfig();
protected:
    std::vector<std::string> Read();
    void Write(const std::string& description, const std::vector<std::string>& data);
private:
    std::string m_filePath;
};

#endif/*CONFIG_H*/