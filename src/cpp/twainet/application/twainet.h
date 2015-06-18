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

// ModuleName cannot have symbol '.'
// String version of ModuleName has the form: name.host.suffix
// All connections(server, client, tunnel) are also modules whose name is:
//     server - ServerName.<sessionId>.
//     client - ClientName.<sessionId>.
//     tunnel - <sessionId>..
extern "C"
{
	namespace Twainet
	{
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
			UNKNOWN = 0,
			LOCAL = 1,
			EXTERNAL = 2,
			RELAY = 3,
			PPP = 4
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

		// Initialize library
		EXPORT_FUNC void InitLibrary(const TwainetCallback& twainet);
		// Create new module
		// isCoordinator - type of module
		// Coordinator - the main module to which all must be connected.
		//               Only the coordinator knows what modules are available on the local machine
		// All modules, when they was created, are only available on the local machine
		EXPORT_FUNC Twainet::Module CreateModule(const ModuleName& moduleName, bool isCoordinator);

		//Delete module
		EXPORT_FUNC void DeleteModule(const Module module);
		
		// Create server that is available outside machine
		EXPORT_FUNC void CreateServer(const Module module, int port);
		
		// Connect to server. If connection is successful will be created server module
		EXPORT_FUNC void ConnectToServer(const Module module, const char* host, int port);
		
		// Disconnect from server. Server module will be destroyed
		EXPORT_FUNC void DisconnectFromServer(const Module module);
		
		// Disconnect from client. Client module will be destroyed
		EXPORT_FUNC void DisconnectFromClient(const Module module, const char* sessionId);
		
		// Connect to module
		EXPORT_FUNC void ConnectToModule(const Module module, const ModuleName& moduleName);
		
		// Disconnect from module
		EXPORT_FUNC void DisconnectFromModule(const Module module, const ModuleName& moduleName);
		
		// Create tunnel between client modules
		// sessionId - module name of other client(ClientName.<sessionId>.)
		// If connection is successful will be created tunnel module
		EXPORT_FUNC void CreateTunnel(const Module module, const char* sessionId, Twainet::TypeConnection type = Twainet::UNKNOWN);
		
		// Destroy tunnel between client modules
		// sessionId - module name of other client(ClientName.<sessionId>.)
		// Tunnel module will be destroyed
		EXPORT_FUNC void DisconnectTunnel(const Module module, const char* sessionId);
		
		// Send message to other module
		// msg - message
		// message has path. Path is a chain of modules through which will pass a message.
		EXPORT_FUNC void SendMessage(const Module module, const Message& msg);
		
		//Get module name
		EXPORT_FUNC ModuleName GetModuleName(const Module module);
		
		// Get session id(module name of client or server connection)
		EXPORT_FUNC const char* GetSessionId(const Module module);
		
		// Get list of existing module names that is available in current module
		EXPORT_FUNC int GetExistingModules(const Module module, ModuleName* modules, int& sizeModules);

		//Set tunnel's type that have to create between two clients
		EXPORT_FUNC void SetTunnelType(const Module module, const char* oneSessionId, const char* twoSessionId, TypeConnection type);
	}
};

#endif/*TWAINET_H*/