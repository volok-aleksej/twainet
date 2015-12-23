#include "config.h"
#include "common/dir.h"

Config::Config()
: m_file(Dir::GetConfigDir() + "/twainet.conf")
{
	m_file.Load();
}

Config::~Config()
{
	m_file.Save();
}
	
void Config::SetGoogleAccountName(const std::string& accountName)
{
	m_file.setString("google", "accountName", accountName.c_str());
}

std::string Config::GetGoogleAccountName()
{
	return m_file.getString("google", "accountName");
}

void Config::SetTrustedFileName(const std::string& fileName)
{
	m_file.setString("trusted", "fileName", fileName.c_str());
}

std::string Config::GetTrustedFileName()
{
	return m_file.getString("trusted", "fileName", "twainet.trusted");
}
