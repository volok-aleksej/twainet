#include "config.h"
#include "common/dir.h"
#include "common/common.h"

Config::Config()
: m_file(Dir::GetConfigDir() + "/twainet.conf")
{
	m_file.Load();
}

Config::~Config()
{
	m_file.Save();
}

void Config::SetTrustedFileName(const std::string& fileName)
{
	m_file.setString("common", "trustedFileName", fileName.c_str());
}

std::string Config::GetTrustedFileName()
{
	return m_file.getString("common", "trustedFileName", "twainet.trusted");
}

int Config::GetLocalServerPort()
{
	return m_file.getLong("common", "localPort", g_localServerPort);
}

void Config::SetLocalServerPort(int port)
{
	m_file.setLong("common", "localPort", port);
}

