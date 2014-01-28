#ifndef TSTRING_H
#define TSTRING_H

#include <string>

#ifndef TCHAR
#	ifdef _UNICODE
#		define TCHAR wchar_t
#	else
	#	define TCHAR char;
#	endif/*_UNICODE*/
#endif/*TCHAR*/

namespace std
{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif/*_UNICODE*/
};

void g_mbstowcs(std::wstring& dest, const std::string& src);
void g_wcstombs(std::string& dest, const std::wstring& src);


#endif/*TSTRING_H*/