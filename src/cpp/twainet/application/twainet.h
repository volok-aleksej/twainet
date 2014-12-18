#ifndef TWAINET_H
#define TWAINET_H

#ifdef SendMessage
#undef SendMessage
#endif/*SendMessage*/

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


		struct TwainetCallback
		{
			void (_stdcall *OnServerConnected)(const char* sessionId);
			void (_stdcall *OnClientConnected)(const char* sessionId);
			void (_stdcall *OnClientDisconnected)(const char* sessionId);
			void (_stdcall *OnModuleConnected)(const char* moduleId);
			void (_stdcall *OnModuleDisconnected)(const char* moduleId);
			void (_stdcall *OnTunnelConnected)(const char* sessionId);
			void (_stdcall *OnTunnelDisconnected)(const char* sessionId);
			void (_stdcall *OnMessageRecv)(const Message& msg);
		};

		struct Module
		{
			void* m_pModule;
			bool m_bIsCoordinator;
			int m_serverPort;
			char m_serverHost[256];
			char m_moduleName[256];
		};
	}

	void _stdcall InitLibrary(const Twainet::TwainetCallback& twainet);
	Twainet::Module _stdcall CreateModule(const char* moduleName, bool isCoordinator);
	void _stdcall DeleteModule(const Twainet::Module& module);
	void _stdcall CreateServer(const Twainet::Module& module);
	void _stdcall ConnectToServer(const Twainet::Module& module);
	void _stdcall ConnectToModule(const Twainet::Module& module, const char* moduleName);
	void _stdcall CreateTunnel(const Twainet::Module& module, const char* sessionId);
	void _stdcall SendMessage(const Twainet::Module& module, const Twainet::Message& msg);
};

#endif/*TWAINET_H*/