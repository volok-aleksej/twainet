// dllmain.cpp : Defines the entry point for the DLL application.
#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
#endif/*_WIN32_WINNT*/
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include "libppp.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern "C" int (DLLCALL CreatePPPSocket)(const char* id)
{
	return 0;
}

extern "C" bool (DLLCALL PPPListen)(int socket)
{
	return false;
}

extern "C" int (DLLCALL PPPAccept)(int socket)
{
	return 0;
}

extern "C" bool (DLLCALL CreatePPPTunnel)(int socket)
{
	return false;
}

extern "C" bool (DLLCALL DestroyPPPTunnel)(int socket)
{
	return false;
}

extern "C" int (DLLCALL SendPPPData)(int socket, const char* data, int len)
{
	return 0;
}

extern "C" int (DLLCALL RecvPPPData)(int socket, char* data, int len)
{
	return 0;
}

extern "C" char* (DLLCALL GetPPPSocketId)(int socket)
{
	return "";
}

extern "C" char* (DLLCALL GetOwnId)()
{
	return "";
}

extern "C" int (DLLCALL GetAvailableIdCount)()
{
	return 0;
}

extern "C" char* (DLLCALL GetAvailableId)(int number)
{
	return "";
}
