#ifndef TWAINET_H
#define TWAINET_H

#ifdef SendMessage
#undef SendMessage
#endif/*SendMessage*/

#ifdef WIN32
#	define TWAINET_CALL __stdcall
#	ifdef TWAINET_EXPORT
#		define TWAINET_FUNC __declspec(dllexport)
#	else
#		define TWAINET_FUNC __declspec(dllimport) 
#	endif // TWAINET_EXPORT
#else
#	define TWAINET_CALL
#	define TWAINET_FUNC extern
#endif/*WIN32*/


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
		struct UserPassword
		{
			char m_user[MAX_NAME_LENGTH];
			char m_pass[MAX_NAME_LENGTH];
		};

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
			void (TWAINET_CALL *OnServerConnected)(Module module, const char* sessionId);
			void (TWAINET_CALL *OnServerDisconnected)(Module module);
			void (TWAINET_CALL *OnServerCreationFailed)(Module module);
			void (TWAINET_CALL *OnClientConnected)(Module module, const char* sessionId);
			void (TWAINET_CALL *OnClientDisconnected)(Module module, const char* sessionId);
			void (TWAINET_CALL *OnClientConnectionFailed)(Module module);
			void (TWAINET_CALL *OnModuleConnected)(Module module, const ModuleName& moduleId);
			void (TWAINET_CALL *OnModuleDisconnected)(Module module, const ModuleName& moduleId);
			void (TWAINET_CALL *OnModuleConnectionFailed)(Module module, const ModuleName& moduleName);
			void (TWAINET_CALL *OnModuleCreationFailed)(Module module);
			void (TWAINET_CALL *OnTunnelConnected)(Module module, const char* sessionId, TypeConnection type);
			void (TWAINET_CALL *OnTunnelDisconnected)(Module module, const char* sessionId);
			void (TWAINET_CALL *OnTunnelCreationFailed)(Module module, const char* sessionId);
			void (TWAINET_CALL *OnMessageRecv)(Module module, const Message& msg);
		};

		// Initialize library
		TWAINET_FUNC void InitLibrary(const TwainetCallback& twainet);

		//Free Library
		TWAINET_FUNC void FreeLibrary();

		// Create new module
		// isCoordinator - type of module
		// Coordinator - the main module to which all must be connected.
		//               Only the coordinator knows what modules are available on the local machine
		// All modules, when they was created, are only available on the local machine
		TWAINET_FUNC Twainet::Module CreateModule(const ModuleName& moduleName, bool isCoordinator, bool isPPPListener);

		//Delete module
		TWAINET_FUNC void DeleteModule(const Module module);
		
		// Create server that is available outside machine
		TWAINET_FUNC void CreateServer(const Module module, int port);
		
		// Connect to server. If connection is successful will be created server module
		TWAINET_FUNC void ConnectToServer(const Module module, const char* host, int port, const UserPassword& userPassword);
		
		// Disconnect from server. Server module will be destroyed
		TWAINET_FUNC void DisconnectFromServer(const Module module);
		
		// Disconnect from client. Client module will be destroyed
		TWAINET_FUNC void DisconnectFromClient(const Module module, const char* sessionId);
		
		// Connect to module
		TWAINET_FUNC void ConnectToModule(const Module module, const ModuleName& moduleName);
		
		// Disconnect from module
		TWAINET_FUNC void DisconnectFromModule(const Module module, const ModuleName& moduleName);
		
		// Create tunnel between client modules
		// sessionId - module name of other client(ClientName.<sessionId>.)
		// If connection is successful will be created tunnel module
		TWAINET_FUNC void CreateTunnel(const Module module, const char* sessionId, Twainet::TypeConnection type = Twainet::UNKNOWN);
		
		// Destroy tunnel between client modules
		// sessionId - module name of other client(ClientName.<sessionId>.)
		// Tunnel module will be destroyed
		TWAINET_FUNC void DisconnectTunnel(const Module module, const char* sessionId);
		
		// Send message to other module
		// msg - message
		// message has path. Path is a chain of modules through which will pass a message.
		TWAINET_FUNC void SendMessage(const Module module, const Message& msg);
		
		//Get module name
		TWAINET_FUNC ModuleName GetModuleName(const Module module);
		
		// Get session id(module name of client or server connection)
		TWAINET_FUNC const char* GetSessionId(const Module module);
		
		// Get list of existing module names that is available in current module
		TWAINET_FUNC int GetExistingModules(const Module module, ModuleName* modules, int& sizeModules);

		// Set tunnel's type that have to create between two clients
		TWAINET_FUNC void SetTunnelType(const Module module, const char* oneSessionId, const char* twoSessionId, TypeConnection type);

		// Set user list for login operation on server
		TWAINET_FUNC void SetUsersList(const Module module, const UserPassword* users, int sizeUsers);

		// Use server and tunnel connections throw proxy server
		TWAINET_FUNC void UseProxy(const Module module, const char* host, int port, const UserPassword& userPassword);

		// Use standart connections
		TWAINET_FUNC void UseStandartConnections(const Module module);
	}
};

#endif/*TWAINET_H*/