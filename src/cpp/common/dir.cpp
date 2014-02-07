#include "dir.h"
#include <direct.h>
#include "utils/path_parser.h"

#ifdef WIN32
#include <windows.h>
#include <Shlobj.h>
#include <Shlwapi.h>

#define getcwd _getcwd
#endif

std::string Dir::GetCurrentDir()
{
	char dirname[500];
	getcwd(dirname, 500);
	return dirname;
}

std::string Dir::GetProcessDir()
{
#ifdef WIN32
	char filename[500];
	GetModuleFileNameA(NULL, filename, 500);
	PathParser parser(filename);
	return parser.GetFolder();
#else
	return "";
#endif
}

std::string Dir::GetCommonDir()
{
#ifdef WIN32
	char pszPath[MAX_PATH + 1] = {0};
	HRESULT res = SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, pszPath);
	return pszPath;
#else
	return "/usr/local/share";
#endif
}
