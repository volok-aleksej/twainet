#ifndef TUNNEL_SERVER_LISTEN_ADDRESS_H
#define TUNNEL_SERVER_LISTEN_ADDRESS_H

#include "connector_lib/thread/address.h"

class TunnelServerListenAddress : public Address
{
public:
	std::string m_sessionIdOne;
	std::string m_sessionIdTwo;
	std::string m_id;
	SocketFactory* m_socketFactory;

	void operator = (const TunnelServerListenAddress& address)
	{
		Address::operator = (address);
		m_socketFactory = address.m_socketFactory;
		m_sessionIdOne = address.m_sessionIdOne;
		m_sessionIdTwo = address.m_sessionIdTwo;
		m_id = address.m_id;
	}
};

#endif/*TUNNEL_SERVER_LISTEN_ADDRESS_H*/