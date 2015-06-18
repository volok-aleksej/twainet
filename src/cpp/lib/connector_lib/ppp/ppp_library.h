#ifndef PPP_LIBRARY_H
#define PPP_LIBRARY_H

#include <windows.h>
#include <string>
#include "common\singleton.h"
#include "libppp\libppp.h"

class PPPLibrary : public Singleton<PPPLibrary>
{
public:
	PPPLibrary();
	~PPPLibrary();

	int CreateSocket(const std::string& sessionId);
	bool Connect(int socket);
	bool Disconnect(int socket);
	int Send(int socket, const char* data, int len);
	int Recv(int socket, char* data, int len);
private:
	HMODULE m_hLibPPP;
	CreatePPPSocket createPPPSocket;
	CreatePPPTunnel createPPPTunnel;
	DestroyPPPTunnel destroyPPPTunnel;
	SendPPPData sendPPPData;
	RecvPPPData recvPPPData;
};

#endif/*PPP_LIBRARY_H*/