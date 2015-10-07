#include "external_connect_thread.h"
#include "common/common_func.h"
#include "connector_lib/socket/udp_socket.h"
#include "connector_lib/socket/udt_socket.h"
#include "connector_lib/socket/socket_factories.h"
#include "connector_lib/connector/connector_factory.h"
#include "connector_lib/message/connector_messages.h"

ExternalConnectThread::ExternalConnectThread(const ConnectAddress& address)
: ConnectThread(address), m_udpSocket(0), m_isChangeAddress(false)
{
}

ExternalConnectThread::~ExternalConnectThread()
{
	CSLocker lock(&m_csSocket);
	if(m_udpSocket)
	{
		delete m_udpSocket;
		m_udpSocket = 0;
	}
}

void ExternalConnectThread::Stop()
{
	StopThread();
	m_csSocket.Enter();
	if(m_udpSocket)
	{
		m_udpSocket->Close();
	}
	m_csSocket.Leave();

	ConnectThread::Stop();
}

void ExternalConnectThread::ChangeConnectAddress(const ConnectAddress& address)
{
	delete m_address.m_connectorFactory;
	delete m_address.m_socketFactory;

	m_address = address;
	m_isChangeAddress = true;
}

void ExternalConnectThread::ThreadFunc()
{
	m_udpSocket = m_address.m_socketFactory->CreateSocket();
	UDPSocket* socket = dynamic_cast<UDPSocket*>(m_udpSocket);
	if(!socket || !socket->Bind(m_address.m_localIP, m_address.m_localPort))
	{
		SignalError();
		return;
	}

	socket->SetSendAddr(m_address.m_ip, m_address.m_port);
	std::string data = m_address.m_moduleName;
	int size = m_address.m_moduleName.size();
	while(!IsStop())
	{
		if (!socket->Send((char*)&size, sizeof(size)) ||
			!socket->Send((char*)data.c_str(), data.size() * sizeof(char)))
		{
			SignalError();
			return;
		}

		if(m_isChangeAddress)
		{
			break;
		}

		sleep(1000);
	}

	UDTSecureSocketFactory* udtSocketFactory = dynamic_cast<UDTSecureSocketFactory*>(m_address.m_socketFactory);
	if(!udtSocketFactory)
	{
		SignalError();
		return;
	}


	udtSocketFactory->SetUdpSocket(socket->GetSocket());
	m_csSocket.Enter();
	m_udpSocket = 0;
	m_csSocket.Leave();
	ConnectThread::ThreadFunc();
}

void ExternalConnectThread::SignalError()
{
	m_csSocket.Enter();
	if(m_udpSocket)
	{
		m_udpSocket->Close();
	}
	m_csSocket.Leave();

	ConnectErrorMessage errMsg(m_address.m_moduleName, "", GetError());
	onSignal(errMsg);
}
