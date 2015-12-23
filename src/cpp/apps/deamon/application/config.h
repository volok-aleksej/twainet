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
	void SetGoogleAccountName(const std::string& accountName);
	std::string GetGoogleAccountName();
	void SetTrustedFileName(const std::string& fileName);
	std::string GetTrustedFileName();
private:
	IniFile m_file;
};

#endif/*CONFIG_H*/