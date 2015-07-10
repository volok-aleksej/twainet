// test.cpp : Defines the entry point for the console application.
//
#ifdef WIN32
#	include "stdafx.h"
#	include <windows.h>
#else
#	include <stdlib.h>
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
	Twainet::InitLibrary(tc);
#ifdef DEBUG_1
	std::string guid = CreateGUID();
	Twainet::ModuleName moduleName = {"Coordinator", "", ""};
	Twainet::Module module = Twainet::CreateModule(moduleName, true, false);
#endif/*DEBUG_1*/
	
	Application::GetInstance();
	system("pause");
	Twainet::FreeLibrary();
	return 0;
}				