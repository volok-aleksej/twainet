#ifdef WIN32
#	include <io.h>
#	include <errno.h>
#	define _WINSOCKAPI_   // prevent inclusion of winsock.h, we need winsock2.h
#	include <Windows.h>
#	include <ShlObj.h>
#else
#	include <unistd.h>
#	include <string.h>
#endif

#include "utils/utils.h"

std::string app_self_name()
{
#ifdef WIN32
	char tmp[MAX_PATH*2] = {0};
	::GetModuleFileNameA(NULL, tmp, MAX_PATH*2);

	std::vector<std::string> vstr = CommonUtils::DelimitString(tmp, "\\");
	if(vstr.empty())
		vstr = CommonUtils::DelimitString(tmp, "/");

	if(vstr.empty())
		return  "";
	
	return (vstr.end() - 1)->c_str();
#elif __APPLE__
	std::string strname = "";
	int mib[5] = {CTL_KERN, KERN_ARGMAX, 0};
	size_t len =  sizeof(int);
	size_t nmax = 0;
	sysctl((int*)mib, 2 , &nmax, &len, NULL, 0);

	char* progargv = (char* )malloc(nmax + 1);
	mib[1] = KERN_PROCARGS;
	mib[2] = getpid();
	char szrealpath[1024] = {0};
	int n = sysctl((int*)mib, 3 , progargv, &nmax, NULL, 0);
	if (n == 0)
	{	    
		realpath(progargv, szrealpath);
	}	
	free(progargv);
	for (int i = strlen(szrealpath); i > 0; i--)
	{
		if (szrealpath[i] == '/')
		{
			strname = szrealpath + i +1;
			break;
		}
	}
	return strname;
#else
	std::string strname;
	char szBuf[1024] = {0};
	ssize_t n = readlink("/proc/self/exe", szBuf, sizeof(szBuf));
	if(n > 0 && n < sizeof(szBuf))
	{
		for(int i = strlen(szBuf) - 1; i > 0; i--)
		{
			if(szBuf[i] == '/')
			{
			  strname = szBuf + i + 1;
			  break;
			}
		}
	}
	return strname;
#endif/*WIN32*/	
}