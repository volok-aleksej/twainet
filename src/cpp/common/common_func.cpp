#ifdef WIN32
#include <objbase.h>
#else
#include <errno.h>
#endif

#include "common_func.h"


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

std::string RandString(int size)
{
	std::string GUID = CreateGUID();
	size_t charN = 0;
	char erazeChars[] = {'{', '}', '-'};
	while(charN < 3)
	{
		size_t pos = GUID.find(erazeChars[charN]);
		if(pos == -1)
		{
			charN++;
			continue;
		}

		GUID.erase(pos, 1);
	}

	return size < GUID.size() ? std::string(GUID.begin(), GUID.begin() + size) : GUID;
}

int GetError()
{
#ifdef WIN32
	return ::GetLastError();
#else
	return errno;
#endif
}
