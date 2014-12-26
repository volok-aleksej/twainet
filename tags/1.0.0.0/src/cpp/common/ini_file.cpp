#include "ini_file.h"

IniFile::IniFile(const std::string& filePath)
: m_filePath(filePath)
{
}

IniFile::~IniFile()
{
}

bool IniFile::getBool(const char* section, const char* key, bool defVal)
{
    return m_iniFile.GetBoolValue(section, key, defVal);
}

bool IniFile::setBool(const char* section, const char* key, bool value)
{
	return SI_OK == m_iniFile.SetBoolValue(section, key, value);
}

long IniFile::getLong(const char* section, const char* key, long defVal)
{
    return m_iniFile.GetLongValue(section, key, defVal);
}

bool IniFile::setLong(const char* section, const char* key, long value)
{
	return SI_OK == m_iniFile.SetLongValue(section, key, value);
}

const char* IniFile::getString(const char* section, const char* key, const char* defVal)
{
    const char* res  = m_iniFile.GetValue(section, key, defVal);
	if (!res)
	{
		return "";
	}
	
	return res;
}

bool IniFile::setString(const char* section, const char* key, const char* value)
{
	return SI_OK == m_iniFile.SetValue(section, key, value);
}

bool IniFile::Load()
{
    return m_iniFile.LoadFile(m_filePath.c_str()) == SI_OK;
}

bool IniFile::Save()
{
    bool ok = m_iniFile.SaveFile(m_filePath.c_str()) == SI_OK;
#ifndef WIN32
	//change file mode so others could read it
	chmod(m_filePath.c_str(), S_IRUSR |S_IWUSR |S_IRGRP|S_IWGRP|S_IWOTH|S_IROTH);
#endif
	
	return ok;
}