#ifndef LIBPPP_H
#define LIBPPP_H


extern "C"
{
#ifdef LIBPPP_EXPORTS
	#define EXPORT_FUNC __declspec(dllexport)
	#define DLLCALL		__cdecl
#else
	#define EXPORT_FUNC typedef
	#define DLLCALL		__cdecl *
#endif // LIBPPP_EXPORT

	EXPORT_FUNC int (DLLCALL CreatePPPSocket)(const char* id);
	EXPORT_FUNC bool (DLLCALL PPPListen)(int socket);
	EXPORT_FUNC int (DLLCALL PPPAccept)(int socket);
	EXPORT_FUNC bool (DLLCALL CreatePPPTunnel)(int socket);
	EXPORT_FUNC bool (DLLCALL DestroyPPPTunnel)(int socket);
	EXPORT_FUNC int (DLLCALL SendPPPData)(int socket, const char* data, int len);
	EXPORT_FUNC int (DLLCALL RecvPPPData)(int socket, char* data, int len);
	EXPORT_FUNC char* (DLLCALL GetPPPSocketId)(int socket);
	EXPORT_FUNC char* (DLLCALL GetOwnId)();
	EXPORT_FUNC int (DLLCALL GetAvailableIdCount)();
	EXPORT_FUNC char* (DLLCALL GetAvailableId)(int number);
};

#endif/*LIBPPP_H*/