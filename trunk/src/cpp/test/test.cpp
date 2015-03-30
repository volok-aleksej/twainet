// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string.h>
#include <iostream>
#include <windows.h>
#include "twainet\application\twainet.h"

void _stdcall OnModuleCreationFailed(Twainet::Module module)
{
	printf("Module creation failed - %s\n", Twainet::GetModuleName(module));
}

void _stdcall OnServerCreationFailed(Twainet::Module module)
{
	printf("Server creation failed, module - %s\n", Twainet::GetModuleName(module));
}

void _stdcall OnTunnelCreationFailed(Twainet::Module module, const char* sessionId)
{
	printf("Tunnel creation failed, from %s to %s\n", Twainet::GetSessionId(module), sessionId);
}

void __stdcall OnServerConnected(Twainet::Module module, const char* sessionId)
{
	static std::string moduleName;
	if(moduleName.empty())
		moduleName = sessionId;
	else
		Twainet::CreateTunnel(module, moduleName.c_str());
	printf("server connected - %s\n", sessionId);
}

void __stdcall OnClientConnected(Twainet::Module module, const char* sessionId)
{
	printf("client connected - %s\n", sessionId);
}

void __stdcall OnClientDisconnected(Twainet::Module module, const char* sessionId)
{
	printf("client disconnected - %s\n", sessionId);
}

void _stdcall OnClientConnectionFailed(Twainet::Module module)
{
	printf("client connection failed - %s\n", Twainet::GetModuleName(module));
}

void _stdcall OnServerDisconnected(Twainet::Module module)
{
	printf("server disconnected\n");
}

void __stdcall OnModuleConnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	printf("module connected - %s.%s.%s\n", moduleId.m_name, moduleId.m_host, moduleId.m_suffix);
}

void __stdcall OnModuleDisconnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	printf("module disconnected - %s.%s.%s\n", moduleId.m_name, moduleId.m_host, moduleId.m_suffix);
}

void _stdcall OnModuleConnectionFailed(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	printf("module connection failed - %s.%s.%s\n", moduleId.m_name, moduleId.m_host, moduleId.m_suffix);
}

void __stdcall OnTunnelConnected(Twainet::Module module, const char* sessionId, Twainet::TypeConnection type)
{
	printf("tunnel connected - %s type - %d\n", sessionId, type);
	Twainet::Message msg;
	Twainet::ModuleName moduleName = {0};
	strcpy_s(moduleName.m_name, MAX_NAME_LENGTH, sessionId);
	msg.m_path = &moduleName;
	msg.m_pathLen = 1;
	msg.m_data = "hello world";
	msg.m_dataLen = strlen("hello world");
	msg.m_typeMessage = "string";
	Twainet::SendMessage(module, msg);
}

void __stdcall OnTunnelDisconnected(Twainet::Module module, const char* sessionId)
{
	printf("tunnel disconnected - %s\n", sessionId);
}

bool bFinish;

void __stdcall OnMessageRecv(Twainet::Module module, const Twainet::Message& msg)
{
	static int i = 0;
	if(++i == 2)
	{
		bFinish = true;
	}

	std::string data(msg.m_data, msg.m_dataLen);
	printf("message receive from ");
	for(int i = 0; i < msg.m_pathLen; i++)
		printf("%s.%s.%s->", msg.m_path[i].m_name, msg.m_path[i].m_host, msg.m_path[i].m_suffix);
	printf("- %s\n", data.c_str());
}


int _tmain(int argc, _TCHAR* argv[])
{
	bFinish = false;
	Twainet::TwainetCallback tc = {	&OnServerConnected, &OnServerDisconnected, &OnServerCreationFailed,
									&OnClientConnected, &OnClientDisconnected, &OnClientConnectionFailed,
									&OnModuleConnected,	&OnModuleDisconnected, &OnModuleConnectionFailed, &OnModuleCreationFailed,
									&OnTunnelConnected,	&OnTunnelDisconnected, &OnTunnelCreationFailed,
									&OnMessageRecv};
	Twainet::InitLibrary(tc);
	Twainet::ModuleName moduleName = {"Server Module", "", ""};
	Twainet::Module module1 = Twainet::CreateModule(moduleName, true);
	Twainet::ModuleName moduleName1 = {"Client Module", "", ""};
	Twainet::Module module2 = Twainet::CreateModule(moduleName1, false);
	Twainet::CreateServer(module1, 8124);
	Twainet::ConnectToServer(module2, "127.0.0.1", 8124);
	Twainet::ConnectToServer(module1, "127.0.0.1", 8124);

	while(!bFinish){Sleep(200);}

	Twainet::ModuleName moduleName2 = {"Client Module1", "", ""};
	Twainet::ConnectToModule(module1, moduleName2);
	Twainet::CreateTunnel(module1, "Tunnel Module");
	system("pause");
	return 0;
}				