// test.cpp : Defines the entry point for the console application.
//
#ifdef WIN32
#	include "stdafx.h"
#	include <windows.h>
#else
#	include <stdlib.h>
#	include <stdio.h>
#endif/*WIN32*/
#include <string.h>
#include <iostream>
#include "twainet/application/twainet.h"
#include "Application.h"
#include "common/common_func.h"
#include "thread_lib/thread/thread_manager.h"

#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif/*WIN32*/
{
	printf("begin\n");
	Twainet::InitLibrary(tc);	
	Twainet::UseLog("/home/avolokitin/twainet.log");
	ApplicationTest::GetInstance();
#ifdef WIN32
	system("pause");
#else
	getchar();
#endif/*WIN32*/
	ApplicationTest::GetInstance().Join();
	Twainet::FreeLibrary();
	return 0;
}				