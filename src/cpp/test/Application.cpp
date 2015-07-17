#ifdef WIN32
#	include "stdafx.h"
#else
#	include <string.h>
#	include <stdio.h>
#endif/*WIN32*/
#include "Application.h"
#include "common/common_func.h"
#include <string>
#include <time.h>

Twainet::TwainetCallback tc = {&ApplicationTest::OnServerConnected, &ApplicationTest::OnServerDisconnected, &ApplicationTest::OnServerCreationFailed,
							   &ApplicationTest::OnClientConnected, &ApplicationTest::OnClientDisconnected, &ApplicationTest::OnClientConnectionFailed,
							   &ApplicationTest::OnModuleConnected,	&ApplicationTest::OnModuleDisconnected, &ApplicationTest::OnModuleConnectionFailed,
							   &ApplicationTest::OnModuleCreationFailed, &ApplicationTest::OnTunnelConnected, &ApplicationTest::OnTunnelDisconnected,
							   &ApplicationTest::OnTunnelCreationFailed, &ApplicationTest::OnMessageRecv};

void TWAINET_CALL ApplicationTest::OnModuleCreationFailed(Twainet::Module module)
{
	ApplicationTest::GetInstance().onModuleCreationFailed(module);
}

void TWAINET_CALL ApplicationTest::OnServerCreationFailed(Twainet::Module module)
{
	ApplicationTest::GetInstance().onServerCreationFailed(module);
}

void TWAINET_CALL ApplicationTest::OnTunnelCreationFailed(Twainet::Module module, const char* sessionId)
{
	ApplicationTest::GetInstance().onTunnelCreationFailed(module, sessionId);
}

void TWAINET_CALL ApplicationTest::OnServerConnected(Twainet::Module module, const char* sessionId)
{
	ApplicationTest::GetInstance().onServerConnected(module, sessionId);
}

void TWAINET_CALL ApplicationTest::OnClientConnected(Twainet::Module module, const char* sessionId)
{
	ApplicationTest::GetInstance().onClientConnected(module, sessionId);
}

void TWAINET_CALL ApplicationTest::OnClientDisconnected(Twainet::Module module, const char* sessionId)
{
	ApplicationTest::GetInstance().onClientDisconnected(module, sessionId);
}

void TWAINET_CALL ApplicationTest::OnClientConnectionFailed(Twainet::Module module)
{
	ApplicationTest::GetInstance().onClientConnectionFailed(module);
}

void TWAINET_CALL ApplicationTest::OnServerDisconnected(Twainet::Module module)
{
	ApplicationTest::GetInstance().onServerDisconnected(module);
}

void TWAINET_CALL ApplicationTest::OnModuleConnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	ApplicationTest::GetInstance().onModuleConnected(module, moduleId);
}

void TWAINET_CALL ApplicationTest::OnModuleDisconnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	ApplicationTest::GetInstance().onModuleDisconnected(module, moduleId);
}

void TWAINET_CALL ApplicationTest::OnModuleConnectionFailed(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	ApplicationTest::GetInstance().onModuleConnectionFailed(module, moduleId);
}

void TWAINET_CALL ApplicationTest::OnTunnelConnected(Twainet::Module module, const char* sessionId, Twainet::TypeConnection type)
{
	ApplicationTest::GetInstance().onTunnelConnected(module, sessionId, type);
}

void TWAINET_CALL ApplicationTest::OnTunnelDisconnected(Twainet::Module module, const char* sessionId)
{
	ApplicationTest::GetInstance().onTunnelDisconnected(module, sessionId);
}

void TWAINET_CALL ApplicationTest::OnMessageRecv(Twainet::Module module, const Twainet::Message& msg)
{
	ApplicationTest::GetInstance().onMessageRecv(module, msg);
}

ApplicationTest::ApplicationTest()
	: m_isStop(false)
{
	Start();
}

ApplicationTest::~ApplicationTest()
{
	Join();
}

void ApplicationTest::OnStop()
{
}

void ApplicationTest::OnStart()
{
}

void ApplicationTest::ThreadFunc()
{
	std::string guid = CreateGUID();
	memset(&m_moduleName, 0, sizeof(m_moduleName));
#ifdef WIN32
	strcpy_s(m_moduleName.m_name, MAX_NAME_LENGTH, guid.c_str());
#else
	strcpy(m_moduleName.m_name, guid.c_str());
#endif/*WIN32*/
	m_module = Twainet::CreateModule(m_moduleName, false, true);
	Twainet::UserPassword user = {"rmmp2plogin", "rmmp2paccess"};
	Twainet::ConnectToServer(m_module, "rmm1-dev.comodormm.com", 1054, user);
	while(!m_isStop){Thread::sleep(200);}
}

void ApplicationTest::Stop()
{
	m_isStop = true;
}

void ApplicationTest::onModuleCreationFailed(Twainet::Module module)
{
	m_isStop = true;
}

void ApplicationTest::onServerCreationFailed(Twainet::Module module)
{
	m_isStop = false;
}

void ApplicationTest::onTunnelCreationFailed(Twainet::Module module, const char* sessionId)
{
	printf("tunnel creation failed - %s\n", sessionId);
}

void ApplicationTest::onServerConnected(Twainet::Module module, const char* sessionId)
{
	printf("server connected - %s\n", sessionId);
}

void ApplicationTest::onClientConnected(Twainet::Module module, const char* sessionId)
{
	printf("client connected - %s\n", sessionId);
}

void ApplicationTest::onClientDisconnected(Twainet::Module module, const char* sessionId)
{
	printf("client disconnected - %s\n", sessionId);
}

void ApplicationTest::onClientConnectionFailed(Twainet::Module module)
{
	printf("client connection failed\n");
}

void ApplicationTest::onServerDisconnected(Twainet::Module module)
{
	printf("server disconnected\n");
}

void ApplicationTest::onModuleConnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	if(module == m_module)
	{
		printf("module connected - %s.%s\n", moduleId.m_name, moduleId.m_host);
		if (strcmp(moduleId.m_name, "Client") == 0)
		{
			Twainet::ModuleName modules[50] = {0};
			int size = 50;
			size = Twainet::GetExistingModules(m_module, modules, size);
			std::string ownSessionId = Twainet::GetSessionId(m_module);
			for(int i = 0; i < size; i++)
			{
				if (!ownSessionId.empty() &&
					strlen(modules[i].m_host) != 0 &&
					strcmp(modules[i].m_host, ownSessionId.c_str()) != 0 &&
					strcmp(modules[i].m_name, "Client") == 0)
				{
					//Twainet::SetTunnelType(m_module, ownSessionId.c_str(), modules[i].m_host, Twainet::PPP);
					Twainet::CreateTunnel(m_module, modules[i].m_host, Twainet::EXTERNAL);
				}
			}
		}
		else
		{
		}
	}
}

void ApplicationTest::onModuleDisconnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	printf("module disconnected - %s\n", moduleId.m_name);
}

void ApplicationTest::onModuleConnectionFailed(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	if(module == m_module)
		printf("module connection failed - %s\n", moduleId.m_name);
}

void ApplicationTest::onTunnelConnected(Twainet::Module module, const char* sessionId, Twainet::TypeConnection type)
{
	printf("tunnel connection - %s type - %d\n", sessionId, type);
//#ifdef DEBUG_1
//			time_t t;
//			time(&t);
//			tm* Tm = localtime(&t);
//			char data[50] = {0};
//			strftime(data, 50, "%H::%M::%S", Tm);
//			printf("begin time: %s\n", data);
//			for(int i = 0; i < 263400; i++)
//			{
//				char* data = new char[8192];
//				Twainet::Message msg = {0};
//				Twainet::ModuleName moduleName = {"Tunnel", "", ""};
//				strcpy(moduleName.m_host, sessionId);
//				msg.m_path = &moduleName;
//				msg.m_pathLen = 1;
//				msg.m_typeMessage = "Data";
//				msg.m_data = data;
//				msg.m_dataLen = 8192;
//				Twainet::SendMessage(m_module, msg);
//				delete data;
//			}
//			t;
//			time(&t);
//			Tm = localtime(&t);
//			memset(data, 0, 50);
//			strftime(data, 50, "%H::%M::%S", Tm);
//			printf("end time: %s\n", data);
//#endif/*DEBUG_1*/
}

void ApplicationTest::onTunnelDisconnected(Twainet::Module module, const char* sessionId)
{
	printf("tunnel disconnected - %s\n", sessionId);
}

void ApplicationTest::onMessageRecv(Twainet::Module module, const Twainet::Message& msg)
{
	static int dataLen = 0;
	dataLen += msg.m_dataLen;
#ifdef WIN32
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = {0, 0};
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hStdOut, &csbi);
	csbi.dwCursorPosition.X = 0;
    SetConsoleCursorPosition(hStdOut, csbi.dwCursorPosition);
#endif
	printf("datalen: %u", dataLen);
}