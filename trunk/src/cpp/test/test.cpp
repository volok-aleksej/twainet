// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string.h>
#include <iostream>
#include "twainet\application\twainet.h"

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

void __stdcall OnMessageRecv(Twainet::Module module, const Twainet::Message& msg)
{
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

	Twainet::TwainetCallback tc = { &OnServerConnected, &OnServerDisconnected,
									&OnClientConnected, &OnClientDisconnected,
									&OnModuleConnected,	&OnModuleDisconnected,
									&OnTunnelConnected,	&OnTunnelDisconnected,
									&OnMessageRecv};
	Twainet::InitLibrary(tc);
	Twainet::Module module1 = Twainet::CreateModule("Server Module1", true);
	Twainet::Module module2 = Twainet::CreateModule("Client Module1", false);
	Twainet::CreateServer(module1, 8124);
	Twainet::ConnectToServer(module2, "127.0.0.1", 8124);
	Twainet::ConnectToServer(module1, "127.0.0.1", 8124);
	system("pause");
	return 0;
}				