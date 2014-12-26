#ifdef WIN32
#include <objbase.h>
#else
#endif

#include "guid_generator.h"


std::string CreateGUID()
{
	std::string guidStr;
#ifdef WIN32
	std::wstring guidWStr;
	guidWStr.resize(50);
	GUID guid;
	CoCreateGuid(&guid);
	StringFromGUID2(guid, (wchar_t*)guidWStr.c_str(), guidWStr.size());
	g_wcstombs(guidStr, guidWStr);
#else
#endif
	return guidStr.c_str();
}