// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "tunnel_lib/module/tunnel_module.h"
#include "ipc_lib/connector/ipc_connector_factory.h"
#include "external\udt4\src\udt.h"

int _tmain(int argc, _TCHAR* argv[])
{
//	WSADATA wd;
//	WSAStartup(2, &wd);
	UDT::startup();

	TunnelModule module1(IPCObjectName("Server Module"), new IPCConnectorFactory<IPCConnector>(IPCObjectName("Server Module")));
	module1.StartAsCoordinator();
	module1.StartServer(8124);
	module1.Connect("127.0.0.1", 8124);

	TunnelModule module2(IPCObjectName("Client Module1"), new IPCConnectorFactory<IPCConnector>(IPCObjectName("Client Module1")));
	module2.Start();
	module2.Connect("127.0.0.1", 8124);

	system("pause");

	module2.InitNewTunnel(module1.GetSessionId());
	
	system("pause");

	Test msg;
	IPCObjectName ipcName3(ClientServerModule::m_serverIPCName, module2.GetSessionId());
	IPCObjectName ipcName2(module1.GetSessionId());
	module2.SendMsg(msg, 2, &ipcName3, &ipcName2);
	module2.SendMsg(msg, 1, &ipcName3);
	module2.SendMsg(msg, 1, &ipcName2);
	
	system("pause");
	module2.Disconnect();
	module2.DestroyTunnel(module1.GetSessionId());
	
	system("pause");
	printf("\n");
	UDT::cleanup();
	return 0;
}

