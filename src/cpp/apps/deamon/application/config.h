#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include "common/ini_file.h"
#include "common/singleton.h"

class Config : public Singleton<Config>
{
private:
	friend class Singleton<Config>;
	Config();
	virtual ~Config();
	
public:	
	void SetTrustedFileName(const std::string& fileName);
	std::string GetTrustedFileName();
	int GetLocalServerPort();
	void SetLocalServerPort(int port);
private:
	IniFile m_file;
};

#endif/*CONFIG_H*/