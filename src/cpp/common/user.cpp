#include "user.h"
#include <stdio.h>
#ifdef WIN32
	#include <windows.h>
#else
	#include <unistd.h>
	#include <pwd.h>
	#include <string.h>
#endif/*WIN32*/

std::string GetUserName()
{
#ifdef WIN32
	char user[250];
	DWORD len = sizeof(user);
	::GetUserNameA(user, &len);
	return user;
#else
	register struct passwd *pw;
	register uid_t uid;
	uid = geteuid ();
	pw = getpwuid (uid);
	return pw->pw_name;
#endif/*WIN32*/
}
