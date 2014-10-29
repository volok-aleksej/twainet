// dllmain.cpp : Defines the entry point for the DLL application.
#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
#endif/*_WIN32_WINNT*/
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <winsock2.h>
#include <windows.h>
#include "udt.h"
#include "twainet.h"
#include "application.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		UDT::startup();
		break;

	case DLL_PROCESS_DETACH:
		UDT::cleanup();
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

extern "C" void Twainet::InitLibrary(const Twainet::TwainetCallback& twainet)
{
	Application::GetInstance().Init(twainet);
}

extern "C" void Twainet::CreateServer()
{
}

extern "C" void Twainet::ConnectToServer(const wchar_t* host)
{
}

extern "C" void Twainet::CreateModule(const wchar_t* moduleName, bool isCoordinator)
{
}

extern "C" void Twainet::ConnectToModule(const wchar_t* moduleName)
{
}

extern "C" void Twainet::CreateTunnel(const wchar_t* sessionId)
{
}

extern "C" void Twainet::SendMessage(const Twainet::Message& msg)
{
}