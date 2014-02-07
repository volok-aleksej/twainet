#ifndef INI_FILE_H
#define INI_FILE_H

#ifdef WIN32
#define   SI_HAS_WIDE_FILE
#endif
#include "SimpleIni.h"

class IniFile
{
public:
	IniFile(const std::string& filePath);
	~IniFile();

	bool getBool(const char* section, const char* key, bool defVal = false);
	bool setBool(const char* section, const char* key, bool value);
	long getLong(const char* section, const char* key, long defVal = 0);
	bool setLong(const char* section, const char* key, long value);
	const char* getString(const char* section, const char* key, const char* defVal = NULL);
	bool setString(const char* section, const char* key, const char* value);

	bool Load();
	bool Save();
private:
	CSimpleIniA m_iniFile;
	std::string m_filePath;
};

#endif/*INI_FILE_H*/