#include "user.h"
#ifdef WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif/*WIN32*/

std::string GetUserName()
{
	char user[250];
#ifdef WIN32
	DWORD len = sizeof(user);
	::GetUserName(user, &len);
#else
	getlogin_r(user, 250);
#endif/*WIN32*/
	return user;
}
