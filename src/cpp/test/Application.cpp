#include "stdafx.h"
#include "Application.h"
#include "common\guid_generator.h"
#include <string>
#include <time.h>

Twainet::TwainetCallback tc = {&Application::OnServerConnected, &Application::OnServerDisconnected, &Application::OnServerCreationFailed,
							   &Application::OnClientConnected, &Application::OnClientDisconnected, &Application::OnClientConnectionFailed,
							   &Application::OnModuleConnected,	&Application::OnModuleDisconnected, &Application::OnModuleConnectionFailed,
							   &Application::OnModuleCreationFailed, &Application::OnTunnelConnected, &Application::OnTunnelDisconnected,
							   &Application::OnTunnelCreationFailed, &Application::OnMessageRecv};

void _stdcall Application::OnModuleCreationFailed(Twainet::Module module)
{
	Application::GetInstance().onModuleCreationFailed(module);
}

void _stdcall Application::OnServerCreationFailed(Twainet::Module module)
{
	Application::GetInstance().onServerCreationFailed(module);
}

void _stdcall Application::OnTunnelCreationFailed(Twainet::Module module, const char* sessionId)
{
	Application::GetInstance().onTunnelCreationFailed(module, sessionId);
}

void _stdcall Application::OnServerConnected(Twainet::Module module, const char* sessionId)
{
	Application::GetInstance().onServerConnected(module, sessionId);
}

void _stdcall Application::OnClientConnected(Twainet::Module module, const char* sessionId)
{
	Application::GetInstance().onClientConnected(module, sessionId);
}

void _stdcall Application::OnClientDisconnected(Twainet::Module module, const char* sessionId)
{
	Application::GetInstance().onClientDisconnected(module, sessionId);
}

void _stdcall Application::OnClientConnectionFailed(Twainet::Module module)
{
	Application::GetInstance().onClientConnectionFailed(module);
}

void _stdcall Application::OnServerDisconnected(Twainet::Module module)
{
	Application::GetInstance().onServerDisconnected(module);
}

void _stdcall Application::OnModuleConnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	Application::GetInstance().onModuleConnected(module, moduleId);
}

void _stdcall Application::OnModuleDisconnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	Application::GetInstance().onModuleDisconnected(module, moduleId);
}

void _stdcall Application::OnModuleConnectionFailed(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	Application::GetInstance().onModuleConnectionFailed(module, moduleId);
}

void _stdcall Application::OnTunnelConnected(Twainet::Module module, const char* sessionId, Twainet::TypeConnection type)
{
	Application::GetInstance().onTunnelConnected(module, sessionId, type);
}

void _stdcall Application::OnTunnelDisconnected(Twainet::Module module, const char* sessionId)
{
	Application::GetInstance().onTunnelDisconnected(module, sessionId);
}

void _stdcall Application::OnMessageRecv(Twainet::Module module, const Twainet::Message& msg)
{
	Application::GetInstance().onMessageRecv(module, msg);
}

Application::Application()
	: m_isStop(false)
{
	Start();
}

Application::~Application()
{
	m_isStop = true;
}

void Application::OnStop()
{
}

void Application::OnStart()
{
}

void Application::ThreadFunc()
{
	std::string guid = CreateGUID();
	memset(&m_moduleName, 0, sizeof(m_moduleName));
	strcpy_s(m_moduleName.m_name, MAX_NAME_LENGTH, guid.c_str());
	m_module = Twainet::CreateModule(m_moduleName, false);
#ifdef DEBUG_1
	Twainet::CreateServer(m_module, 1054);
	Twainet::ConnectToServer(m_module, "127.0.0.1", 1054);
#else
	Twainet::ConnectToServer(m_module, "127.0.0.1", 1054);
#endif
	while(!m_isStop){Sleep(200);}
}

void Application::Stop()
{
}

void Application::onModuleCreationFailed(Twainet::Module module)
{
	m_isStop = true;
}

void Application::onServerCreationFailed(Twainet::Module module)
{
	m_isStop = false;
}

void Application::onTunnelCreationFailed(Twainet::Module module, const char* sessionId)
{
}

void Application::onServerConnected(Twainet::Module module, const char* sessionId)
{
	printf("server connected - %s\n", sessionId);
}

void Application::onClientConnected(Twainet::Module module, const char* sessionId)
{
	printf("client connected - %s\n", sessionId);
}

void Application::onClientDisconnected(Twainet::Module module, const char* sessionId)
{
	printf("client disconnected - %s\n", sessionId);
}

void Application::onClientConnectionFailed(Twainet::Module module)
{
}

void Application::onServerDisconnected(Twainet::Module module)
{
	printf("server disconnected\n");
}

void Application::onModuleConnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	if(module == m_module)
	{
		printf("module connected - %s.%s\n", moduleId.m_name, moduleId.m_host);
		if (strcmp(moduleId.m_name, "Client") == 0)
		{
			Twainet::ModuleName modules[10] = {0};
			int size = 10;
			size = Twainet::GetExistingModules(m_module, modules, size);
			std::string ownSessionId = Twainet::GetSessionId(m_module);
			for(int i = 0; i < size; i++)
			{
				if (!ownSessionId.empty() &&
					strlen(modules[i].m_host) != 0 &&
					strcmp(modules[i].m_host, ownSessionId.c_str()) != 0 &&
					strcmp(modules[i].m_name, "Client") == 0)
				{
					Twainet::CreateTunnel(m_module, modules[i].m_host);
				}
			}
		}
		else
		{
		}
	}
}

void Application::onModuleDisconnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	printf("module disconnected - %s\n", moduleId.m_name);
}

void Application::onModuleConnectionFailed(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	if(module == m_module)
		printf("module connection failed - %s\n", moduleId.m_name);
}

void Application::onTunnelConnected(Twainet::Module module, const char* sessionId, Twainet::TypeConnection type)
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

void Application::onTunnelDisconnected(Twainet::Module module, const char* sessionId)
{
	printf("tunnel disconnected - %s\n", sessionId);
}

void Application::onMessageRecv(Twainet::Module module, const Twainet::Message& msg)
{
	static int dataLen = 0;
	dataLen += msg.m_dataLen;
	
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = {0, 0};
    DWORD count;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hStdOut, &csbi);
	csbi.dwCursorPosition.X = 0;
    SetConsoleCursorPosition(hStdOut, csbi.dwCursorPosition);
	printf("datalen: %u", dataLen);
}