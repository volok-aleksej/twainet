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

extern "C"
{
	namespace Twainet
	{
		struct Message
		{	
			const char* m_path;
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
			void (_stdcall *OnModuleConnected)(Module module, const char* moduleId);
			void (_stdcall *OnModuleDisconnected)(Module module, const char* moduleId);
			void (_stdcall *OnModuleCreationFailed)(Module module);
			void (_stdcall *OnTunnelConnected)(Module module, const char* sessionId, TypeConnection type);
			void (_stdcall *OnTunnelDisconnected)(Module module, const char* sessionId);
			void (_stdcall *OnTunnelCreationFailed)(Module module, const char* sessionId);
			void (_stdcall *OnMessageRecv)(Module module, const Message& msg);
		};

		EXPORT_FUNC void InitLibrary(const Twainet::TwainetCallback& twainet);
		EXPORT_FUNC Twainet::Module CreateModule(const char* moduleName, bool isCoordinator);
		EXPORT_FUNC void DeleteModule(const Twainet::Module module);
		EXPORT_FUNC void CreateServer(const Twainet::Module module, int port);
		EXPORT_FUNC void ConnectToServer(const Twainet::Module module, const char* host, int port);
		EXPORT_FUNC void DisconnectFromServer(const Twainet::Module module);
		EXPORT_FUNC void DisconnectFromClient(const Twainet::Module module, const char* sessionId);
		EXPORT_FUNC void ConnectToModule(const Twainet::Module module, const char* moduleName);
		EXPORT_FUNC void DisconnectFromModule(const Twainet::Module module, const char* moduleName);
		EXPORT_FUNC void CreateTunnel(const Twainet::Module module, const char* sessionId);
		EXPORT_FUNC void SendMessage(const Twainet::Module module, const Twainet::Message& msg);
		EXPORT_FUNC const char* GetModuleName(const Twainet::Module module);
		EXPORT_FUNC const char* GetSessionId(const Twainet::Module module);
	}
};

#endif/*TWAINET_H*/