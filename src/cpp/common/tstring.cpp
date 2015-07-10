#ifdef WIN32
	#include <windows.h>
#else
	#include <stdlib.h>
#endif/*WIN32*/

#include "tstring.h"

void g_mbstowcs(std::wstring& dest, const std::string& src)
{
	wchar_t* ldest;
	unsigned int rv = 0, c = 0;
	bool end = false;

	while (c < 2)
	{
		ldest = (wchar_t*)dest.c_str();
#ifdef WIN32
		rv = MultiByteToWideChar(CP_ACP, 0, (char*)src.c_str(), (int)src.size(), ldest, rv);
#else
		rv = mbstowcs(ldest, src.c_str(), rv);
#endif
		dest.resize(rv);
		c++;
	}
}

void g_wcstombs(std::string& dest, const std::wstring& src)
{
	const wchar_t* lsrc;
	int rv = 0, c = 0;

	while (c < 2)
	{
		lsrc = src.c_str();
#ifdef WIN32
		rv = WideCharToMultiByte(CP_ACP, 0, lsrc, (int)src.size(), (char*)dest.c_str(), rv, 0, 0);
#else
		rv = wcstombs((char*)dest.c_str(), lsrc, rv);
#endif
		dest.resize(rv);
		c++;
	}
}
