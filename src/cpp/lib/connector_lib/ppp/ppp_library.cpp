#include "ppp_library.h"

PPPLibrary::PPPLibrary()
{
	m_hLibPPP = LoadLibrary(L"libppp.dll");
	if(m_hLibPPP)
	{
		createPPPSocket = (::CreatePPPSocket)GetProcAddress(m_hLibPPP, "CreatePPPSocket");
		pppListen = (::PPPListen)GetProcAddress(m_hLibPPP, "PPPListen");
		pppAccept = (::PPPAccept)GetProcAddress(m_hLibPPP, "PPPAccept");
		createPPPTunnel = (::CreatePPPTunnel)GetProcAddress(m_hLibPPP, "CreatePPPTunnel");
		destroyPPPTunnel = (::DestroyPPPTunnel)GetProcAddress(m_hLibPPP, "DestroyPPPTunnel");
		sendPPPData = (::SendPPPData)GetProcAddress(m_hLibPPP, "SendPPPData");
		recvPPPData = (::RecvPPPData)GetProcAddress(m_hLibPPP, "RecvPPPData");
		getSocketId = (::GetPPPSocketId)GetProcAddress(m_hLibPPP, "GetPPPSocketId");
		getOwnId = (::GetOwnId)GetProcAddress(m_hLibPPP, "GetOwnId");
		getAvailableIdCount = (::GetAvailableIdCount)GetProcAddress(m_hLibPPP, "GetAvailableIdCount");
		getAvailableId = (::GetAvailableId)GetProcAddress(m_hLibPPP, "GetAvailableId");
	}
}

PPPLibrary::~PPPLibrary()
{
}

int PPPLibrary::CreateSocket(const std::string& sessionId)
{
	if(m_hLibPPP && createPPPSocket)
	{
		if(sessionId.empty())
			return createPPPSocket(GetOwnId().c_str());
		else
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

bool PPPLibrary::Listen(int socket)
{
	if(m_hLibPPP && pppListen)
	{
		return pppListen(socket);
	}

	return false;
}

int PPPLibrary::Accept(int socket)
{
	if(m_hLibPPP && pppAccept)
	{
		return pppAccept(socket);
	}

	return 0;
}

std::string PPPLibrary::GetOwnId()
{	
	if(m_hLibPPP && getOwnId)
	{
		return getOwnId();
	}
	return "";
}

std::string PPPLibrary::GetSocketId(int socket)
{
	if(m_hLibPPP && getSocketId)
	{
		return getSocketId(socket);
	}
	return "";
}

int PPPLibrary::GetAvailableIdCount()
{
	if(m_hLibPPP && getAvailableIdCount)
	{
		return getAvailableIdCount();
	}
	return 0;
}

std::string PPPLibrary::GetAvailableId(int number)
{
	if(m_hLibPPP && getAvailableId)
	{
		return getAvailableId(number);
	}
	return "";
}
