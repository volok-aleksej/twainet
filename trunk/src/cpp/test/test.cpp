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

void __stdcall OnModuleConnected(Twainet::Module module, const char* moduleId)
{
	printf("module connected - %s\n", moduleId);
}

void __stdcall OnModuleDisconnected(Twainet::Module module, const char* moduleId)
{
	printf("module disconnected - %s\n", moduleId);
}

void _stdcall OnModuleConnectionFailed(Twainet::Module module, const char* moduleId)
{
	printf("module connection failed - %s\n", moduleId);
}

void __stdcall OnTunnelConnected(Twainet::Module module, const char* sessionId, Twainet::TypeConnection type)
{
	printf("tunnel connected - %s type - %d\n", sessionId, type);
	Twainet::Message msg;
	msg.m_path = sessionId;
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
	printf("message receive from %s - %s\n", msg.m_path, data.c_str());
}


int _tmain(int argc, _TCHAR* argv[])
{
//	Test msg;
//	IPCObjectName ipcName3(ClientServerModule::m_serverIPCName, module2.GetSessionId());
//	IPCObjectName ipcName2(module1.GetSessionId());
//	module2.SendMsg(msg, 2, &ipcName3, &ipcName2);
//	module2.SendMsg(msg, 1, &ipcName3);
//	module2.SendMsg(msg, 1, &ipcName2);

	bFinish = false;
	Twainet::TwainetCallback tc = {	&OnServerConnected, &OnServerDisconnected, &OnServerCreationFailed,
									&OnClientConnected, &OnClientDisconnected, &OnClientConnectionFailed,
									&OnModuleConnected,	&OnModuleDisconnected, &OnModuleConnectionFailed, &OnModuleCreationFailed,
									&OnTunnelConnected,	&OnTunnelDisconnected, &OnTunnelCreationFailed,
									&OnMessageRecv};
	Twainet::InitLibrary(tc);
	Twainet::Module module1 = Twainet::CreateModule("Server Module1", true);
	Twainet::Module module2 = Twainet::CreateModule("Client Module1", false);
	Twainet::CreateServer(module1, 8124);
	Twainet::ConnectToServer(module2, "127.0.0.1", 8124);
	Twainet::ConnectToServer(module1, "127.0.0.1", 8124);

	while(!bFinish){Sleep(200);}

	Twainet::ConnectToModule(module1, "Client Module");
	Twainet::CreateTunnel(module1, "Tunnel Module");

	system("pause");
	return 0;
}				