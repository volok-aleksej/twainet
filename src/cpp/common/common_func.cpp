#ifdef WIN32
#include <objbase.h>
#else
#include <errno.h>
#include <uuid/uuid.h>
#include <stdio.h>
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
	StringFromGUID2(guid, (wchar_t*)guidWStr.c_str(), (int)guidWStr.size());
	g_wcstombs(guidStr, guidWStr);
#else
	uuid_t out;
	uuid_generate_time(out);
	for(int i = 0; i < sizeof(out); i++)
	{
	  char num[10] = {0};
	  snprintf(num, 10, "%d", out[i]);
	  guidStr.append(num);
	}
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

	return size < (int)GUID.size() ? std::string(GUID.begin(), GUID.begin() + size) : GUID;
}

int GetError()
{
#ifdef WIN32
	return ::GetLastError();
#else
	return errno;
#endif
}
