#ifdef WIN32
#	ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#		define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
#	endif/*_WIN32_WINNT*/
#	define WIN32_LEAN_AND_MEAN     
#	include <winsock2.h>
#	include <windows.h>
#else
#	include <signal.h>
#endif/*WIN32*/
#include "udt.h"
#include "include/twainet.h"
#include "application.h"
#include "utils/utils.h"
#include "utils/logger.h"

#pragma warning(disable: 4273)

extern "C" void Twainet::InitLibrary(const Twainet::TwainetCallback& twainet)
{
    if(!Application::GetInstance().IsInited()) {
        LOG_INFO("Init twainet library\n");
	UDT::startup();
        Application::GetInstance().Init(twainet);
        ManagersContainer::GetInstance().Start();
    }
#ifndef WIN32
	struct sigaction new_action;
    new_action.sa_handler = SIG_IGN;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;
	sigaction(SIGPIPE, &new_action, NULL);
#endif/*WIN32*/
}

extern "C" bool Twainet::IsLibraryInited()
{
    return Application::GetInstance().IsInited();
}

extern "C" void Twainet::CloseLibrary()
{
    if(Application::GetInstance().IsInited()) {
        Application::GetInstance().Deinit();
	ManagersContainer::GetInstance().Join();
	UDT::cleanup();
    }
}

extern "C" Twainet::Module Twainet::CreateModule(const char* moduleName, IPVersion ipv, bool isCoordinator)
{
	TwainetModule* module = Application::GetInstance().CreateModule(moduleName, ipv);
	isCoordinator ?  module->StartAsCoordinator() : module->Start();
	return (Twainet::Module*)module;
}

extern "C" void Twainet::DeleteModule(const Twainet::Module module)
{
	if(!module)
		return;

	Application::GetInstance().DeleteModule((TwainetModule*)module);
}

extern "C" void Twainet::CreateServer(const Twainet::Module module, int port, IPVersion ipv, bool local)
{
	if(!module)
		return;

	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->StartServer(port, (int)ipv, local);
}

extern "C" void Twainet::ConnectToServer(const Twainet::Module module, const char* host, int port, const UserPassword& userPassword)
{
	if(!module)
		return;

	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->SetUserName(userPassword.m_user);
	twainetModule->SetPassword(userPassword.m_pass);
	twainetModule->Connect(host, port);
}

extern "C" void Twainet::DisconnectFromServer(const Twainet::Module module)
{
	if(!module)
		return;

	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->Disconnect();
}

extern "C" void Twainet::DisconnectFromClient(const Twainet::Module module, const char* sessionId)
{
	if(!module)
		return;

	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->DisconnectModule(IPCObjectName(ClientServerModule::m_clientIPCName, sessionId));
}

extern "C" void Twainet::ConnectToModule(const Twainet::Module module, const char* moduleName)
{
	if(!module)
		return;

	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->ConnectTo(IPCObjectName(moduleName));
}

extern "C" void DisconnectFromModule(const Twainet::Module module, const char* moduleName)
{
	if(!module)
		return;

	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->DisconnectModule(IPCObjectName(moduleName));
}

extern "C" void Twainet::CreateTunnel(const Twainet::Module module, const char* sessionId, Twainet::TypeConnection type)
{
	if(!module)
		return;

	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->InitNewTunnel(sessionId, (TunnelConnector::TypeConnection)type);
}

extern "C" void Twainet::DisconnectTunnel(const Twainet::Module module, const char* sessionId)
{
	if(!module)
		return;

	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->DestroyTunnel(sessionId);
}

extern "C" void Twainet::SendMessage(const Twainet::Module module, const Twainet::Message& msg)
{
	if(!module)
		return;


	TwainetModule* twainetModule = (TwainetModule*)module;
	IPCMessage message;
	message.set_message_name(msg.m_typeMessage);
	message.set_message(msg.m_data, msg.m_dataLen);
	std::vector<IPCObjectName> path = twainetModule->GetTargetPath(IPCObjectName(msg.m_target.m_name, msg.m_target.m_host, msg.m_target.m_connId));
	for(std::vector<IPCObjectName>::iterator it = path.begin();
	    it != path.end(); it++)
	{
		*message.add_ipc_path() = *it;
	}
	
	IPCMessageSignal msgSignal(message);
	twainetModule->SendMsg(msgSignal);
}

extern "C" bool Twainet::SendSyncMessage(const Twainet::Module module, const Twainet::Message& msg, Twainet::Message& resp)
{
    if(!module)
		return false;
    
    TwainetModule* twainetModule = (TwainetModule*)module;
	IPCMessage message;
	message.set_message_name(msg.m_typeMessage);
	message.set_message(msg.m_data, msg.m_dataLen);
	std::vector<IPCObjectName> path = twainetModule->GetTargetPath(IPCObjectName(msg.m_target.m_name, msg.m_target.m_host, msg.m_target.m_connId));
	for(std::vector<IPCObjectName>::iterator it = path.begin();
	    it != path.end(); it++)
	{
		*message.add_ipc_path() = *it;
	}
	
	IPCMessageSignal msgSignal(message);
    std::string data;
	if (twainetModule->SendSyncMsg(msgSignal, resp.m_typeMessage, IPCObjectName(resp.m_target.m_name, resp.m_target.m_host, resp.m_target.m_connId), data) && 
        !data.empty()) {
        resp.m_data = new char[data.size()];
        resp.m_dataLen = data.size();
        memcpy((void*)resp.m_data, data.c_str(), data.size());
        return true;
    }
    
    return false;
}

extern "C" void FreeMessage(Twainet::Message& msg)
{
    if(msg.m_data){
        delete[] msg.m_data;
    }
}

extern "C" Twainet::ModuleName Twainet::GetModuleName(const Twainet::Module module)
{
	if(!module)
	{
		ModuleName moduleName = {0};
		return moduleName;
	}

	TwainetModule* twainetModule = (TwainetModule*)module;
	const IPCObjectName& name = twainetModule->GetModuleName();
	Twainet::ModuleName retName = {0};
#ifdef WIN32
	strcpy_s(retName.m_name, MAX_NAME_LENGTH, name.module_name().c_str());
	strcpy_s(retName.m_host, MAX_NAME_LENGTH, name.host_name().c_str());
	strcpy_s(retName.m_connId, MAX_NAME_LENGTH, name.conn_id().c_str());
#else
	strcpy(retName.m_name, name.module_name().c_str());
	strcpy(retName.m_host, name.host_name().c_str());
	strcpy(retName.m_connId, name.conn_id().c_str());
#endif/*WIN32*/
	return retName;
}

extern "C" void ChangeModuleName(const Twainet::Module module, const char* moduleName)
{
	if(!module)
	{
		return;
	}

	TwainetModule* twainetModule = (TwainetModule*)module;
	IPCObjectName ipcModuleName(moduleName);
	twainetModule->UpdateModuleName(ipcModuleName);
}
		
extern "C" const char* Twainet::GetSessionId(const Twainet::Module module)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	return twainetModule->GetSessionId().c_str();
}

extern "C" int Twainet::GetExistingModules(const Twainet::Module module, Twainet::ModuleName* modules, int& sizeModules)
{
	if(!module)
		return 0;

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
#ifdef WIN32
		strcpy_s(modules[i].m_name, MAX_NAME_LENGTH, it->module_name().c_str());
		strcpy_s(modules[i].m_host, MAX_NAME_LENGTH, it->host_name().c_str());
		strcpy_s(modules[i].m_connId, MAX_NAME_LENGTH, it->conn_id().c_str());
#else
		strcpy(modules[i].m_name, it->module_name().c_str());
		strcpy(modules[i].m_host, it->host_name().c_str());
		strcpy(modules[i].m_connId, it->conn_id().c_str());
#endif/*WIN32*/
	}
	return objects.size();
}

extern "C" void Twainet::SetTunnelType(const Twainet::Module module, const char* oneSessionId, const char* twoSessionId, Twainet::TypeConnection type)
{
	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->SetTypeTunnel(oneSessionId, twoSessionId, (TunnelConnector::TypeConnection)type);
}

extern "C" void Twainet::SetAvailableTunnelTypes(const Module module, TypeConnection* types, int sizeTypes)
{
    TwainetModule* twainetModule = (TwainetModule*)module;
    std::vector<TunnelType> tunnelTypes;
    for(int i = 0; i < sizeTypes; i++)
    {
        tunnelTypes.push_back((TunnelType)types[i]);
    }
    twainetModule->SetTunnelAvailableType(tunnelTypes);
}

extern "C" void Twainet::SetUsersList(const Twainet::Module module, const Twainet::UserPassword* users, int sizeUsers)
{
	if(!module)
		return;

	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->ClearUsers();
	for(int i = 0; i < sizeUsers; i++)
	{
		twainetModule->AddUser(users[i].m_user, users[i].m_pass);
	}
}

extern "C" void Twainet::UseProxy(const Twainet::Module module, const char* host, int port, const Twainet::UserPassword& userPassword)
{
	if(!module)
		return;

	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->UseProxy(host, port);
	twainetModule->SetProxyUserName(userPassword.m_user);
	twainetModule->SetProxyPassword(userPassword.m_pass);
}

extern "C" void Twainet::UseStandartConnections(const Twainet::Module module)
{
	if(!module)
		return;

	TwainetModule* twainetModule = (TwainetModule*)module;
	twainetModule->UseStandartConnections();
}

extern "C" void Twainet::UseLog(char* logFileName)
{
	Logger::GetInstance().SetLogFile(logFileName);
}

extern "C" void Twainet::CreateInternalConnection(const Twainet::Module module, const Twainet::ModuleName& moduleName, const char* ip, int port)
{
	if(!module)
		return;
	
	TwainetModule* twainetModule = (TwainetModule*)module;
	IPCObjectName ipcModuleName(moduleName.m_name, moduleName.m_host, moduleName.m_connId);
	twainetModule->CreateInternalConnection(ipcModuleName, ip, port);
}

extern "C" int Twainet::GetInternalConnections(const Twainet::Module module, Twainet::ModuleName* connections, int& sizeConnections)
{
	if(!module)
		return 0;

	TwainetModule* twainetModule = (TwainetModule*)module;
	std::vector<IPCObjectName> objects = twainetModule->GetInternalConnections();
	if(sizeConnections < (int)objects.size())
	{
		sizeConnections = objects.size();
		return 0;
	}

	int i = 0;
	for(std::vector<IPCObjectName>::iterator it = objects.begin();
		it != objects.end(); it++, i++)
	{
#ifdef WIN32
		strcpy_s(connections[i].m_name, MAX_NAME_LENGTH, it->module_name().c_str());
		strcpy_s(connections[i].m_host, MAX_NAME_LENGTH, it->host_name().c_str());
		strcpy_s(connections[i].m_connId, MAX_NAME_LENGTH, it->conn_id().c_str());
#else
		strcpy(connections[i].m_name, it->module_name().c_str());
		strcpy(connections[i].m_host, it->host_name().c_str());
		strcpy(connections[i].m_connId, it->conn_id().c_str());
#endif/*WIN32*/
	}
	return objects.size();
}

extern "C" int Twainet::GetModuleNameString(const ModuleName& moduleName, char* str, int& strlen)
{
	IPCObjectName name(moduleName.m_name, moduleName.m_host, moduleName.m_connId);
	std::string strModuleName = name.GetModuleNameString();
	if(strlen < (int)strModuleName.size())
	{
		strlen = strModuleName.size();
		return 0;
	}
	
#ifdef WIN32
		strcpy_s(str, strlen, strModuleName.c_str());
#else
		strcpy(str, strModuleName.c_str());
#endif/*WIN32*/
	return strlen;
}
