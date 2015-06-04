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

#pragma warning(disable: 4273)

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

extern "C" Twainet::Module Twainet::CreateModule(const Twainet::ModuleName& moduleName, bool isCoordinator)
{
	TwainetModule* module = Application::GetInstance().CreateModule(moduleName);
	isCoordinator ?  module->StartAsCoordinator() : module->Start();
	return (Twainet::Module*)module;
}

extern "C" void Twainet::DeleteModule(const Twainet::Module module)
{
	Application::GetInstance().DeleteModule((TwainetModule*)module);
}

extern "C" void Twainet::CreateServer(const Twainet::Module module, int port)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->StartServer(port);
}

extern "C" void Twainet::ConnectToServer(const Twainet::Module module, const char* host, int port)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->Connect(host, port);
}

extern "C" void Twainet::DisconnectFromServer(const Twainet::Module module)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->Disconnect();
}

extern "C" void Twainet::DisconnectFromClient(const Twainet::Module module, const char* sessionId)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->DisconnectModule(IPCObjectName(ClientServerModule::m_clientIPCName, sessionId));
}

extern "C" void Twainet::ConnectToModule(const Twainet::Module module, const Twainet::ModuleName& moduleName)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->ConnectTo(IPCObjectName(moduleName.m_name, moduleName.m_host, moduleName.m_suffix));
}

extern "C" void DisconnectFromModule(const Twainet::Module module, const Twainet::ModuleName& moduleName)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->DisconnectModule(IPCObjectName(moduleName.m_name, moduleName.m_host, moduleName.m_suffix));
}

extern "C" void Twainet::CreateTunnel(const Twainet::Module module, const char* sessionId)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->InitNewTunnel(sessionId);
}

extern "C" void Twainet::DisconnectTunnel(const Twainet::Module module, const char* sessionId)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->DestroyTunnel(sessionId);
}

extern "C" void Twainet::SendMessage(const Twainet::Module module, const Twainet::Message& msg)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	IPCMessage message;
	message.set_message_name(msg.m_typeMessage);
	message.set_message(msg.m_data, msg.m_dataLen);
	for(int i = 0; i < msg.m_pathLen; i++)
	{
		*message.add_ipc_path() = IPCObjectName(msg.m_path[i].m_name, msg.m_path[i].m_host, msg.m_path[i].m_suffix);
	}

	IPCMessageSignal msgSignal(message);
	twainetModule->SendMsg(msgSignal);
}

extern "C" Twainet::ModuleName Twainet::GetModuleName(const Twainet::Module module)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	const IPCObjectName& name = twainetModule->GetModuleName();
	Twainet::ModuleName retName = {0};
	strcpy_s(retName.m_name, MAX_NAME_LENGTH, name.module_name().c_str());
	strcpy_s(retName.m_host, MAX_NAME_LENGTH, name.host_name().c_str());
	strcpy_s(retName.m_suffix, MAX_NAME_LENGTH, name.suffix().c_str());
	return retName;
}

extern "C" const char* Twainet::GetSessionId(const Twainet::Module module)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	return twainetModule->GetSessionId().c_str();
}

extern "C" int Twainet::GetExistingModules(const Twainet::Module module, Twainet::ModuleName* modules, int& sizeModules)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	std::vector<IPCObjectName> objects = twainetModule->GetIPCObjects();
	if(sizeModules < (int)objects.size())
	{
		sizeModules = objects.size();
		return 0;
	}

	int i = 0;
	for(std::vector<IPCObjectName>::iterator it = objects.begin();
		it != objects.end(); it++, i++)
	{
		strcpy_s(modules[i].m_name, MAX_NAME_LENGTH, it->module_name().c_str());
		strcpy_s(modules[i].m_host, MAX_NAME_LENGTH, it->host_name().c_str());
		strcpy_s(modules[i].m_suffix, MAX_NAME_LENGTH, it->suffix().c_str());
	}
	return objects.size();
}