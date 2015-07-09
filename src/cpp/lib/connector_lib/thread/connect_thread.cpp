#include "connect_thread.h"
#include "common/common_func.h"
#include "socket/any_socket.h"
#include "socket/socket_factory.h"
#include "connector/connector.h"
#include "connector/connector_factory.h"
#include "message/connector_messages.h"

ConnectThread::ConnectThread(const ConnectAddress& address)
: m_address(address), m_socket(0)
{
}

ConnectThread::~ConnectThread()
{
	if(m_socket)
	{
		delete m_socket;
		m_socket = 0;
	}
	delete m_address.m_connectorFactory;
	delete m_address.m_socketFactory;
}

void ConnectThread::ThreadFunc()
{
	m_socket = m_address.m_socketFactory->CreateSocket();
	m_socket->Bind(m_address.m_localIP, m_address.m_localPort);

	if(m_socket->Connect(m_address.m_ip, m_address.m_port))
	{
		Connector* connector = m_address.m_connectorFactory->CreateConnector(m_socket);
		connector->SetId(m_address.m_id);
		connector->SetRemoteAddr(m_address.m_ip, m_address.m_port);
		ConnectorMessage msg(connector);
		onSignal(msg);
		m_socket = 0;
	}
	else
	{
		ConnectErrorMessage errMsg(m_address.m_moduleName, "", GetError());
		onSignal(errMsg);
		delete m_socket;
		m_socket = 0;
	}
}

void ConnectThread::Stop()
{
	if (m_socket)
	{
		m_socket->Close();
	}
}
