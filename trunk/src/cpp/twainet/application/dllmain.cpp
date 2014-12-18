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
#include "utils/utils.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		UDT::startup();
		Application::GetInstance();
		break;

	case DLL_PROCESS_DETACH:
		UDT::cleanup();
		Application::GetInstance().Stop();
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

extern "C" void _stdcall InitLibrary(const Twainet::TwainetCallback& twainet)
{
	Application::GetInstance().Init(twainet);
}

extern "C" Twainet::Module _stdcall CreateModule(const char* moduleName, bool isCoordinator)
{
	TwainetModule* module = Application::GetInstance().CreateModule(moduleName);
	Twainet::Module twainetModule = {0};
	if(strlen(moduleName) > sizeof(twainetModule.m_moduleName))
		return twainetModule;
	strcpy(twainetModule.m_moduleName, moduleName);
	twainetModule.m_bIsCoordinator = isCoordinator;
	twainetModule.m_serverPort = 8124;
	twainetModule.m_pModule = module;
	isCoordinator ?  module->StartAsCoordinator() : module->Start();
	return twainetModule;
}

extern "C" void _stdcall DeleteModule(const Twainet::Module& module)
{
	Application::GetInstance().DeleteModule((TwainetModule*)module.m_pModule);
}

extern "C" void _stdcall CreateServer(const Twainet::Module& module)
{
	TwainetModule* twainetModule = (TwainetModule*)module.m_pModule;
	twainetModule->StartServer(module.m_serverPort);
}

extern "C" void _stdcall ConnectToServer(const Twainet::Module& module)
{
	TwainetModule* twainetModule = (TwainetModule*)module.m_pModule;
	twainetModule->Connect(module.m_serverHost, module.m_serverPort);
}

extern "C" void _stdcall ConnectToModule(const Twainet::Module& module, const char* moduleName)
{
	TwainetModule* twainetModule = (TwainetModule*)module.m_pModule;
	twainetModule->ConnectTo(IPCObjectName::GetIPCName(moduleName));
}

extern "C" void _stdcall CreateTunnel(const Twainet::Module& module, const char* sessionId)
{
	TwainetModule* twainetModule = (TwainetModule*)module.m_pModule;
	twainetModule->InitNewTunnel(sessionId);
}

extern "C" void _stdcall SendMessage(const Twainet::Module& module, const Twainet::Message& msg)
{
	TwainetModule* twainetModule = (TwainetModule*)module.m_pModule;
	IPCMessage message;
	message.set_message_name(msg.m_typeMessage);
	message.set_message(msg.m_data, msg.m_dataLen);
	std::vector<std::string> strings = CommonUtils::DelimitString(msg.m_path, "->");
	for(std::vector<std::string>::iterator it = strings.begin(); it != strings.end(); it++)
	{
		*message.add_ipc_path() = IPCObjectName::GetIPCName(*it);
	}

	IPCMessageSignal msgSignal(message);
	twainetModule->SendMsg(msgSignal);
}