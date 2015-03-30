#ifndef TWAINET_H
#define TWAINET_H

#ifdef SendMessage
#undef SendMessage
#endif/*SendMessage*/

#ifdef TWAINET_EXPORT
#define EXPORT_FUNC __declspec(dllexport) 
#else
#define EXPORT_FUNC __declspec(dllimport) 
#endif // TWAINET_EXPORT

#define MAX_NAME_LENGTH 255

extern "C"
{
	namespace Twainet
	{
		//module name cannot have simbol '.'
		struct ModuleName
		{
			char m_name[MAX_NAME_LENGTH];
			char m_host[MAX_NAME_LENGTH];
			char m_suffix[MAX_NAME_LENGTH];
		};

		struct Message
		{	
			const ModuleName* m_path;
			int m_pathLen;
			const char* m_typeMessage;
			const char* m_data;
			int m_dataLen;
		};

		enum TypeConnection
		{
			LOCAL = 1,
			EXTERNAL = 2,
			RELAY = 3
		};

		typedef void* Module;

		struct TwainetCallback
		{
			void (_stdcall *OnServerConnected)(Module module, const char* sessionId);
			void (_stdcall *OnServerDisconnected)(Module module);
			void (_stdcall *OnServerCreationFailed)(Module module);
			void (_stdcall *OnClientConnected)(Module module, const char* sessionId);
			void (_stdcall *OnClientDisconnected)(Module module, const char* sessionId);
			void (_stdcall *OnClientConnectionFailed)(Module module);
			void (_stdcall *OnModuleConnected)(Module module, const ModuleName& moduleId);
			void (_stdcall *OnModuleDisconnected)(Module module, const ModuleName& moduleId);
			void (_stdcall *OnModuleConnectionFailed)(Module module, const ModuleName& moduleName);
			void (_stdcall *OnModuleCreationFailed)(Module module);
			void (_stdcall *OnTunnelConnected)(Module module, const char* sessionId, TypeConnection type);
			void (_stdcall *OnTunnelDisconnected)(Module module, const char* sessionId);
			void (_stdcall *OnTunnelCreationFailed)(Module module, const char* sessionId);
			void (_stdcall *OnMessageRecv)(Module module, const Message& msg);
		};

		EXPORT_FUNC void InitLibrary(const TwainetCallback& twainet);
		EXPORT_FUNC Twainet::Module CreateModule(const ModuleName& moduleName, bool isCoordinator);
		EXPORT_FUNC void DeleteModule(const Module module);
		EXPORT_FUNC void CreateServer(const Module module, int port);
		EXPORT_FUNC void ConnectToServer(const Module module, const char* host, int port);
		EXPORT_FUNC void DisconnectFromServer(const Module module);
		EXPORT_FUNC void DisconnectFromClient(const Module module, const char* sessionId);
		EXPORT_FUNC void ConnectToModule(const Module module, const ModuleName& moduleName);
		EXPORT_FUNC void DisconnectFromModule(const Module module, const ModuleName& moduleName);
		EXPORT_FUNC void CreateTunnel(const Module module, const char* sessionId);
		EXPORT_FUNC void DisconnectTunnel(const Module module, const char* sessionId);
		EXPORT_FUNC void SendMessage(const Module module, const Message& msg);
		EXPORT_FUNC ModuleName GetModuleName(const Module module);
		EXPORT_FUNC const char* GetSessionId(const Module module);
		EXPORT_FUNC int GetExistingModules(const Module module, ModuleName* modules, int& sizeModules);
	}
};

#endif/*TWAINET_H*/