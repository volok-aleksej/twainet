#include "external_recv_thread.h"
#include "connector_lib/socket/udp_socket.h"
#include "connector_lib/message/connector_messages.h"

ExternalRecvThread::ExternalRecvThread(UDPSocket* socket, const std::string& id)
: m_socket(socket), m_id(id)
{
}

ExternalRecvThread::~ExternalRecvThread()
{
	if(m_socket)
	{
		delete m_socket;
		m_socket = 0;
	}
}

void ExternalRecvThread::Stop()
{
	StopThread();

	if (m_socket)
	{
		m_socket->Close();
	}
}

void ExternalRecvThread::ThreadFunc()
{
	int len = 0;
	while(!IsStop())
	{
		if (!m_socket->Recv((char*)&len, sizeof(int)))
		{
			ListenErrorMessage msg(m_id, "", GetLastError());
			onSignal(msg);
			break;
		}

		if(len < 0 || len > 0xffff)
		{
			ListenErrorMessage msg(m_id, "", GetLastError());
			onSignal(msg);
			break;
		}

		std::string data;
		data.resize(len);
		std::string ip;
		int port;
		if(!m_socket->RecvFrom((char*)data.c_str(), len, ip, port))
		{
			ListenErrorMessage msg(m_id, "", GetLastError());
			onSignal(msg);
			break;
		}

		if(m_id == data)
		{
			CreatedListenerMessage msg(ip, port, m_id);
			onSignal(msg);
		}
	}
}