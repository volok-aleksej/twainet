#ifdef WIN32
#	include "stdafx.h"
#else
#	include <string.h>
#	include <stdio.h>
#endif/*WIN32*/
#include "Application.h"
#include "common/common_func.h"
#include "common/common.h"
#include <string>
#include <time.h>

ApplicationTest::ApplicationTest()
{
}

ApplicationTest::~ApplicationTest()
{
}

void ApplicationTest::OnModuleCreationFailed(Twainet::Module module)
{
	Stop();
}

void ApplicationTest::OnServerCreationFailed(Twainet::Module module)
{
	Stop();
}

void ApplicationTest::OnTunnelCreationFailed(Twainet::Module module, const char* sessionId)
{
	printf("tunnel creation failed - %s\n", sessionId);
    if(m_module)
        m_module->OnTunnelCreationFailed(sessionId);
}

void ApplicationTest::OnServerConnected(Twainet::Module module, const char* sessionId)
{
	printf("server connected - %s\n", sessionId);
	if(m_module)
        m_module->OnServerConnected(sessionId);
}

void ApplicationTest::OnClientConnected(Twainet::Module module, const char* sessionId)
{
	printf("client connected - %s\n", sessionId);
	if(m_module)
        m_module->OnClientConnected(sessionId);
}

void ApplicationTest::OnClientDisconnected(Twainet::Module module, const char* sessionId)
{
	printf("client disconnected - %s\n", sessionId);
	if(m_module)
        m_module->OnClientDisconnected(sessionId);
}

void ApplicationTest::OnClientConnectionFailed(Twainet::Module module)
{
	printf("client connection failed\n");
	if(m_module)
        m_module->OnClientConnectionFailed();
}

void ApplicationTest::OnClientAuthFailed(Twainet::Module module)
{
	printf("client authorization failed\n");
	if(m_module)
        m_module->OnClientAuthFailed();
}

void ApplicationTest::OnServerDisconnected(Twainet::Module module)
{
	printf("server disconnected\n");
	if(m_module)
        m_module->OnServerDisconnected();
}

void ApplicationTest::OnModuleConnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	printf("module connected - %s.%s\n", moduleId.m_name, moduleId.m_host);
	if(m_module)
        m_module->OnModuleConnected(moduleId);
}

void ApplicationTest::OnModuleDisconnected(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	printf("module disconnected - %s\n", moduleId.m_name);
	if(m_module)
        m_module->OnModuleDisconnected(moduleId);
}

void ApplicationTest::OnModuleConnectionFailed(Twainet::Module module, const Twainet::ModuleName& moduleId)
{
	printf("module connection failed - %s\n", moduleId.m_name);
	if(m_module)
        m_module->OnModuleConnectionFailed(moduleId);
}

void ApplicationTest::OnTunnelConnected(Twainet::Module module, const char* sessionId, Twainet::TypeConnection type)
{
	printf("tunnel connection - %s type - %d\n", sessionId, type);
	if(m_module)
        m_module->OnTunnelConnected(sessionId, type);
}

void ApplicationTest::OnTunnelDisconnected(Twainet::Module module, const char* sessionId)
{
	printf("tunnel disconnected - %s\n", sessionId);
	if(m_module)
        m_module->OnTunnelDisconnected(sessionId);
}

void ApplicationTest::OnMessageRecv(Twainet::Module module, const Twainet::Message& msg)
{
	char name[255] = {0};
	int size = 255;
	Twainet::GetModuleNameString(msg.m_target, name, size);
	if(m_module)
        m_module->OnMessageRecv(msg);
}

void ApplicationTest::OnInternalConnectionStatusChanged(Twainet::Module module, const char* moduleName, Twainet::InternalConnectionStatus status, int port)
{
	printf("internal connection (id - %s) status changed - %d\n", status);
	if(status == Twainet::OPEN)
		printf("external port available - %d\n", port);
	if(m_module)
        m_module->OnInternalConnectionStatusChanged(moduleName, status, port);
}

void ApplicationTest::OnModuleListChanged(Twainet::Module module)
{
    if(m_module)
        m_module->OnModuleListChanged();
}

void ApplicationTest::InitializeApplication()
{
	m_module = new TestModule;
}

void ApplicationTest::ShutdownApplication()
{
    if(m_module)    
        delete m_module;
}
