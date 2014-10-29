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
			const wchar_t* m_senderId;
			const wchar_t* m_receiverId;
			const wchar_t* m_typeMessage;
			const wchar_t* m_data;
			int m_dataLen;
		};

		typedef void (_stdcall *OnServerConnectedFunction)(const wchar_t* sessionId);
		typedef void (_stdcall *OnClientConnectedFunction)(const wchar_t* sessionId);
		typedef void (_stdcall *OnClientDisconnectedFunction)(const wchar_t* sessionId);
		typedef void (_stdcall *OnModuleConnectedFunction)(const wchar_t* moduleId);
		typedef void (_stdcall *OnModuleDisconnectedFunction)(const wchar_t* moduleId);
		typedef void (_stdcall *OnTunnelConnectedFunction)(const wchar_t* sessionId);
		typedef void (_stdcall *OnTunnelDisconnectedFunction)(const wchar_t* sessionId);
		typedef void (_stdcall *OnMessageRecvFunction)(const Message& msg);

		struct TwainetCallback
		{
			OnServerConnectedFunction*		OnServerConnected;
			OnClientConnectedFunction*		OnClientConnected;
			OnClientDisconnectedFunction*	OnClientDisconnected; 
			OnModuleConnectedFunction*		OnModuleConnected;
			OnModuleDisconnectedFunction*	OnModuleDisconnected;
			OnTunnelConnectedFunction*		OnTunnelConnected;
			OnTunnelDisconnectedFunction*	OnTunnelDisconnected;
			OnMessageRecvFunction*			OnMessageRecv;
		};

		void InitLibrary(const TwainetCallback& twainet);
		void CreateServer();
		void ConnectToServer(const wchar_t* host);
		void CreateModule(const wchar_t* moduleName, bool isCoordinator);
		void ConnectToModule(const wchar_t* moduleName);
		void CreateTunnel(const wchar_t* sessionId);
		void SendMessage(const Message& msg);
	}
};

#endif/*TWAINET_H*/