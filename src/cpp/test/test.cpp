// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string.h>
#include <iostream>
#include <windows.h>
#include "twainet\application\twainet.h"
#include "application.h"
#include "common\guid_generator.h"

int _tmain(int argc, _TCHAR* argv[])
{
	Twainet::InitLibrary(tc);
#ifdef DEBUG_1
	std::string guid = CreateGUID();
	Twainet::ModuleName moduleName = {"Coordinator", "", ""};
	Twainet::Module module = Twainet::CreateModule(moduleName, true);
#endif/*DEBUG_1*/
	
	Application::GetInstance().Join();
	return 0;
}				