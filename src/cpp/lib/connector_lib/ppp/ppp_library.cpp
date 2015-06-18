#include "ppp_library.h"

PPPLibrary::PPPLibrary()
{
	m_hLibPPP = LoadLibrary(L"libppp.dll");
	if(m_hLibPPP)
	{
		createPPPSocket = (CreatePPPSocket)GetProcAddress(m_hLibPPP, "CreatePPPSocket");
		createPPPTunnel = (CreatePPPTunnel)GetProcAddress(m_hLibPPP, "CreatePPPTunnel");
		destroyPPPTunnel = (DestroyPPPTunnel)GetProcAddress(m_hLibPPP, "DestroyPPPTunnel");
		sendPPPData = (SendPPPData)GetProcAddress(m_hLibPPP, "SendPPPData");
		recvPPPData = (RecvPPPData)GetProcAddress(m_hLibPPP, "RecvPPPData");
	}
}

PPPLibrary::~PPPLibrary()
{
}

int PPPLibrary::CreateSocket(const std::string& sessionId)
{
	if(m_hLibPPP && createPPPSocket)
	{
		return createPPPSocket(sessionId.c_str());
	}

	return 0;
}

bool PPPLibrary::Connect(int socket)
{
	if(m_hLibPPP && createPPPTunnel)
	{
		return createPPPTunnel(socket);
	}

	return false;
}

bool PPPLibrary::Disconnect(int socket)
{
	if(m_hLibPPP && destroyPPPTunnel)
	{
		return destroyPPPTunnel(socket);
	}

	return false;
}

int PPPLibrary::Send(int socket, const char* data, int len)
{
	if(m_hLibPPP && sendPPPData)
	{
		return sendPPPData(socket, data, len);
	}

	return -1;
}

int PPPLibrary::Recv(int socket, char* data, int len)
{
	if(m_hLibPPP && recvPPPData)
	{
		return recvPPPData(socket, data, len);
	}

	return -1;
}