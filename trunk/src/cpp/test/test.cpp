// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string.h>
#include <iostream>
#include "twainet\application\twainet.h"

//#include "tunnel_lib/module/tunnel_module.h"
//#include "ipc_lib/connector/ipc_connector_factory.h"
//#include "external\udt4\src\udt.h"

void __stdcall OnServerConnected(const char* sessionId)
{
}

void __stdcall OnClientConnected(const char* sessionId)
{
}

void __stdcall OnClientDisconnected(const char* sessionId)
{
}

void __stdcall OnModuleConnected(const char* moduleId)
{
}

void __stdcall OnModuleDisconnected(const char* moduleId)
{
}

void __stdcall OnTunnelConnected(const char* sessionId)
{
}

void __stdcall OnTunnelDisconnected(const char* sessionId)
{
}

void __stdcall OnMessageRecv(const Twainet::Message& msg)
{
}

int _tmain(int argc, _TCHAR* argv[])
{
//	UDT::startup();
//
//	TunnelModule module1(IPCObjectName("Server Module"), new IPCConnectorFactory<IPCConnector>(IPCObjectName("Server Module")));
//	module1.StartAsCoordinator();
//	module1.StartServer(8124);
//	module1.Connect("127.0.0.1", 8124);
//
//	TunnelModule module2(IPCObjectName("Client Module1"), new IPCConnectorFactory<IPCConnector>(IPCObjectName("Client Module1")));
//	module2.Start();
//	module2.Connect("127.0.0.1", 8124);
//
//	system("pause");
//
//	module2.InitNewTunnel(module1.GetSessionId());
//	
//	system("pause");
//
//	Test msg;
//	IPCObjectName ipcName3(ClientServerModule::m_serverIPCName, module2.GetSessionId());
//	IPCObjectName ipcName2(module1.GetSessionId());
//	module2.SendMsg(msg, 2, &ipcName3, &ipcName2);
//	module2.SendMsg(msg, 1, &ipcName3);
//	module2.SendMsg(msg, 1, &ipcName2);
//	
//	system("pause");
//	module2.Disconnect();
//	module2.DestroyTunnel(module1.GetSessionId());
//	
//	system("pause");
//	printf("\n");
//	UDT::cleanup();

	Twainet::TwainetCallback tc = { &OnServerConnected, &OnClientConnected,
									&OnClientDisconnected, &OnModuleConnected,
									&OnModuleDisconnected, &OnTunnelConnected,
									&OnTunnelDisconnected, &OnMessageRecv};
	InitLibrary(tc);
	Twainet::Module module1 = CreateModule("Server Module1", true);
	module1.m_serverPort = 8124;
	strcpy(module1.m_serverHost, "127.0.0.1");
	Twainet::Module module2 = CreateModule("Client Module1", false);
	module2.m_serverPort = 8124;
	strcpy(module2.m_serverHost, "127.0.0.1");
	CreateServer(module1);
	ConnectToServer(module2);
	system("pause");

	return 0;
}				