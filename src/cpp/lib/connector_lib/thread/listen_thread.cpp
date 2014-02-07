#include "listen_thread.h"
#include "socket/socket_factory.h"
#include "socket/any_socket.h"
#include "connector/connector.h"
#include "connector/connector_factory.h"
#include "message/connector_messages.h"

ListenThread::ListenThread(const ListenAddress& address)
: m_address(address), m_socket(0)
{
}

ListenThread::~ListenThread()
{
	if(m_socket)
	{
		delete m_socket;
		m_socket = 0;
	}
	delete m_address.m_connectorFactory;
	delete m_address.m_socketFactory;
}

void ListenThread::ThreadFunc()
{
	m_socket = m_address.m_socketFactory->CreateSocket();
	if(!m_socket->Bind(m_address.m_localIP, m_address.m_localPort) ||
		!m_socket->Listen(100))
	{
		ListenErrorMessage errMsg(m_address.m_id, "", GetLastError());
		onSignal(errMsg);
		delete m_socket;
		m_socket = 0;
		return;
	}

	std::string ip;
	int port = 0;
	m_socket->GetIPPort(ip, port);
	CreatedListenerMessage clMsg(ip, port, m_address.m_id);
	onSignal(clMsg);

	bool infinite = (m_address.m_acceptCount == -1);
	int count = m_address.m_acceptCount;
	while(!IsStop())
	{
		std::string ip;
		int port;
		int sock = m_socket->Accept(ip, port);
		if(sock != INVALID_SOCKET)
		{
			AnySocket* socket = m_address.m_socketFactory->CreateSocket(sock);
			Connector* connector = m_address.m_connectorFactory->CreateConnector(socket);
			connector->SetId(m_address.m_id);
			connector->SetRemoteAddr(ip, port);
			ConnectorMessage msg(connector);
			onSignal(msg);
		}
		else
		{
			ErrorMessage errMsg("", GetLastError());
			onSignal(errMsg);
			delete m_socket;
			m_socket = 0;
			return;
		}

		if(!infinite && --count <= 0)
		{
			delete m_socket;
			m_socket = 0;
			return;
		}
	}
}

void ListenThread::Stop()
{
	StopThread();

	if(m_socket)
	{
		m_socket->Close();
	}

}