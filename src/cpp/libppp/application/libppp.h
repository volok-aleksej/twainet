#ifndef LIBPPP_H
#define LIBPPP_H


extern "C"
{
#ifdef LIBPPP_EXPORTS
	#define PPP_FUNC __declspec(dllexport)
	#define DLLCALL		__cdecl
#else
	#define PPP_FUNC typedef
	#define DLLCALL		__cdecl *
#endif // LIBPPP_EXPORT

	PPP_FUNC void (DLLCALL InitPPPLibrary)();
	PPP_FUNC void (DLLCALL FreePPPLibrary)();
	PPP_FUNC int (DLLCALL CreatePPPSocket)(unsigned char* id);
	PPP_FUNC bool (DLLCALL PPPListen)(int socket);
	PPP_FUNC int (DLLCALL PPPAccept)(int socket);
	PPP_FUNC bool (DLLCALL CreatePPPTunnel)(int socket);
	PPP_FUNC bool (DLLCALL DestroyPPPTunnel)(int socket);
	PPP_FUNC int (DLLCALL SendPPPData)(int socket, const char* data, int len);
	PPP_FUNC int (DLLCALL RecvPPPData)(int socket, char* data, int len);
	PPP_FUNC unsigned char* (DLLCALL GetPPPSocketId)(int socket);
	PPP_FUNC unsigned char* (DLLCALL GetOwnId)();
	PPP_FUNC int (DLLCALL GetAvailableIdCount)();
	PPP_FUNC unsigned short (DLLCALL GetAvailableId)(int number);
};

#endif/*LIBPPP_H*/