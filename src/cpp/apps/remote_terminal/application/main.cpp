#ifdef WIN32
#	include "stdafx.h"
#	include <windows.h>
#else
#	include <stdlib.h>
#	include <stdio.h>
#endif/*WIN32*/
#include <string.h>
#include <iostream>
#include "include/twainet.h"
#include "application.h"
#include "common/common_func.h"

#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif/*WIN32*/
{
//	printf("begin\n");
	ApplicationTerminal::GetInstance().Run();
	return 0;
}
