// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "tunnel_lib/module/tunnel_module.h"
#include "ipc_lib/connector/ipc_connector_factory.h"

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wd;
	WSAStartup(2, &wd);

	//TunnelModule module1(IPCObjectName("Server Module"), new IPCConnectorFactory<IPCConnector>(IPCObjectName("Server Module")));
	//module1.Start();
	//module1.StartServer(12345);

	//TunnelModule module2(IPCObjectName("Client Module1"), new IPCConnectorFactory<IPCConnector>(IPCObjectName("Client Module1")));
	//module2.Start();
	//module2.Connect("127.0.0.1", 12345);

	//TunnelModule module4(IPCObjectName("Client Module2"), new IPCConnectorFactory<IPCConnector>(IPCObjectName("Client Module2")));
	//module4.Start();
	//module4.Connect("127.0.0.1", 12345);

	//TunnelModule module5(IPCObjectName("Client Module3"), new IPCConnectorFactory<IPCConnector>(IPCObjectName("Client Module3")));
	//module5.Start();
	//TunnelModule module6(IPCObjectName("Client Module3"), new IPCConnectorFactory<IPCConnector>(IPCObjectName("Client Module3")));
	//module6.Start();
	//
	//IPCModule module3(IPCObjectName("IPC Module Coordinator"), new IPCConnectorFactory<IPCConnector>(IPCObjectName("IPC Module Coordinator")));
	//module3.StartAsCoordinator();

	//system("pause");
	//printf("\n");

	//module2.InitNewTunnel(module4.GetSessionId());

	//system("pause");
	//printf("\n");
	//Test msg;
	//IPCObjectName ipcName3(module4.GetSessionId());
	//IPCObjectName ipcName2("Client Module1");
	//IPCObjectName ipcName1("IPC Module Coordinator");
	//module4.SendMsg(msg, 3, &ipcName1, &ipcName2, &ipcName3);

	TunnelModule module2(IPCObjectName("App1"), new IPCConnectorFactory<IPCConnector>(IPCObjectName("App1")));
//	module2.StartAsCoordinator();
	module2.Start();
//	module2.SetUserName("test");
	module2.Connect("127.0.0.1", 8124);
//	module2.Connect("192.168.70.174", 8124);
	system("pause");
	printf("\n");

	module2.UpdateModuleName(IPCObjectName("App"));

//	module2.InitNewTunnel("{5c40c867-6eb0-d097-96a3-1af0cb41c363}");
	
	system("pause");
	printf("\n");
	return 0;
}

