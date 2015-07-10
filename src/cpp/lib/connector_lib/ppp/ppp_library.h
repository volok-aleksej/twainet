#ifndef PPP_LIBRARY_H
#define PPP_LIBRARY_H

#ifdef WIN32
#include <windows.h>
#include <string>
#include "common\singleton.h"
#include "libppp\application\libppp.h"

class PPPLibrary : public Singleton<PPPLibrary>
{
public:
	PPPLibrary();
	~PPPLibrary();
	
	void InitLibrary();
	void FreeLibrary();
	int CreateSocket(const std::string& sessionId);
	bool Connect(int socket);
	bool Disconnect(int socket);
	int Send(int socket, const char* data, int len);
	int Recv(int socket, char* data, int len);
	bool Listen(int socket);
	int Accept(int socket);
	std::string GetSocketId(int socket);
	std::string GetOwnId();
	int GetAvailableIdCount();
	std::string GetAvailableId(int number);
private:
	HMODULE m_hLibPPP;
	::InitPPPLibrary initLibrary;
	::FreePPPLibrary freeLibrary;
	::PPPListen pppListen;
	::PPPAccept pppAccept;
	::CreatePPPSocket createPPPSocket;
	::CreatePPPTunnel createPPPTunnel;
	::DestroyPPPTunnel destroyPPPTunnel;
	::SendPPPData sendPPPData;
	::RecvPPPData recvPPPData;
	::GetPPPSocketId getSocketId;
	::GetOwnId getOwnId;
	::GetAvailableIdCount getAvailableIdCount;
	::GetAvailableId getAvailableId;
};
#endif/*WIN32*/

#endif/*PPP_LIBRARY_H*/