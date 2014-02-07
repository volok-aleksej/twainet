#include "external_listen_thread.h"
#include "../message/tunnel_messages.h"
#include "connector_lib/socket/udp_socket.h"
#include "connector_lib/socket/socket_factories.h"
#include "thread_lib/thread/thread_manager.h"

extern std::vector<std::string> GetLocalIps();

ExternalListenThread::ExternalListenThread(const TunnelServerListenAddress& address)
: m_address(address), m_recvThreadOne(0), m_recvThreadTwo(0)
{
}

ExternalListenThread::~ExternalListenThread()
{
	removeReceiver();

	if(m_recvThreadOne)
	{
		m_recvThreadOne->Stop();
		ThreadManager::GetInstance().AddThread(m_recvThreadOne);
		m_recvThreadOne = 0;
	}

	if(m_recvThreadTwo)
	{
		m_recvThreadTwo->Stop();
		ThreadManager::GetInstance().AddThread(m_recvThreadTwo);
		m_recvThreadTwo = 0;
	}

	delete m_address.m_socketFactory;
}

void ExternalListenThread::Stop()
{
	StopThread();
	
	CSLocker lock(&m_cs);
	if(m_recvThreadOne)
	{
		m_recvThreadOne->Stop();
		ThreadManager::GetInstance().AddThread(m_recvThreadOne);
		m_recvThreadOne = 0;
	}

	if(m_recvThreadTwo)
	{
		m_recvThreadTwo->Stop();
		ThreadManager::GetInstance().AddThread(m_recvThreadTwo);
		m_recvThreadTwo = 0;
	}
}

void ExternalListenThread::ThreadFunc()
{
	std::string ip;
	int port;

	AnySocket* socket = m_address.m_socketFactory->CreateSocket();
	UDPSocket* udpSocket = dynamic_cast<UDPSocket*>(socket);
	if(!socket || !udpSocket->Bind(m_address.m_localIP, m_address.m_localPort))
	{
		delete socket;
		SignalError();
		return;
	}

	udpSocket->GetIPPort(ip, port);
	ip = GetLocalIps()[0];//TODO: use external ip
	CreatedServerListenerMessage clMsg(m_address.m_id, m_address.m_sessionIdOne, ip, port);
	onSignal(clMsg);

	m_recvThreadOne = new ExternalRecvThread(udpSocket, m_address.m_sessionIdOne);
	m_recvThreadOne->addSubscriber(this, SIGNAL_FUNC(this, ExternalListenThread, ListenErrorMessage, onListenErrorMessage));
	m_recvThreadOne->addSubscriber(this, SIGNAL_FUNC(this, ExternalListenThread, CreatedListenerMessage, onCreatedListenerMessage));
	m_recvThreadOne->Start();

	socket = m_address.m_socketFactory->CreateSocket();
	udpSocket = dynamic_cast<UDPSocket*>(socket);
	if(!socket || !udpSocket->Bind(m_address.m_localIP, m_address.m_localPort))
	{
		delete socket;
		SignalError();
		return;
	}

	udpSocket->GetIPPort(ip, port);
	ip = GetLocalIps()[0];//TODO: use external ip
	CreatedServerListenerMessage clMsg1(m_address.m_id, m_address.m_sessionIdTwo, ip, port);
	onSignal(clMsg1);

	m_recvThreadTwo = new ExternalRecvThread(udpSocket, m_address.m_sessionIdTwo);
	m_recvThreadTwo->addSubscriber(this, SIGNAL_FUNC(this, ExternalListenThread, ListenErrorMessage, onListenErrorMessage));
	m_recvThreadTwo->addSubscriber(this, SIGNAL_FUNC(this, ExternalListenThread, CreatedListenerMessage, onCreatedListenerMessage));
	m_recvThreadTwo->Start();

	while(!IsStop())
	{
		if(CheckGetSession())
		{
			for(std::map<std::string, Address>::iterator it = m_addresses.begin();
				it != m_addresses.end(); it++)
			{
				GotExternalAddressMessage msg(m_address.m_id, it->first, it->second.m_localIP, it->second.m_localPort);
				onSignal(msg);
			}
			break;
		}
		sleep(100);
	}
}

bool ExternalListenThread::CheckGetSession()
{
	return m_addresses.size() == 2;
}

void ExternalListenThread::SignalError()
{
	ListenErrorMessage errMsg(m_address.m_id, "", GetLastError());
	onSignal(errMsg);
}

void ExternalListenThread::onListenErrorMessage(const ListenErrorMessage& msg)
{
	{
		CSLocker lock(&m_cs);
		if(m_recvThreadOne)
		{
			m_recvThreadOne->Stop();
			ThreadManager::GetInstance().AddThread(m_recvThreadOne);
			m_recvThreadOne = 0;
		}

		if(m_recvThreadTwo)
		{
			m_recvThreadTwo->Stop();
			ThreadManager::GetInstance().AddThread(m_recvThreadTwo);
			m_recvThreadTwo = 0;
		}
	}

	SignalError();
}

void ExternalListenThread::onCreatedListenerMessage(const CreatedListenerMessage& msg)
{
	Address addr;
	addr.m_localIP = msg.m_ip;
	addr.m_localPort = msg.m_port;
	m_addresses[msg.m_id] = addr;
}