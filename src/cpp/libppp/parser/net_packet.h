#ifndef NET_PACKET_H
#define NET_PACKET_H

#include <vector>
#include <string>
#include "common\ref.h"

class IConnection;

class IConnectionPacket
{
public:
	virtual ~IConnectionPacket(){}

	virtual bool IsConnectionPacket(IConnection* conn) = 0;
	virtual void PacketConnection(IConnection* conn) = 0;
	virtual std::string GetType() = 0;
};

class IConnection
{
public:
	virtual ~IConnection(){}

	bool IsConnectionPacket(IConnectionPacket* packet)
	{
		for(std::vector<std::string>::iterator it = m_packetNames.begin();
			it != m_packetNames.end(); it++)
		{
			if(*it == packet->GetType())
				return true;
		}

		return false;
	}

protected:
	void AddPacketName(IConnectionPacket& packet)
	{
		m_packetNames.push_back(packet.GetType());
	}

private:
	std::vector<std::string> m_packetNames;
};

template<typename Packet, typename Connection, typename void (Connection::*ConnectionFunc)(Packet*)>
class ConnectionPacket : public IConnectionPacket
{
public:
	ConnectionPacket() : m_packet(0){}
	ConnectionPacket(Packet* packet)
		: m_packet((Packet*)packet->Clone()){}
	virtual ~ConnectionPacket()
	{
		if(m_packet)
			delete m_packet;
	}

	virtual bool IsConnectionPacket(IConnection* conn)
	{
		Connection* connection = 0;
		if (conn &&
			(connection = dynamic_cast<Connection*>(conn)) &&
			conn->IsConnectionPacket(this))
		{
			return connection->IsConnectionPacket(m_packet);
		}
		return false;
	}

	virtual void PacketConnection(IConnection* conn)
	{
		if(!conn)
			return;
		Connection* connection = dynamic_cast<Connection*>(conn);
		if(connection)
			(connection->*ConnectionFunc)(m_packet);
	}

	virtual std::string GetType()
	{
		return typeid(Packet).name();
	}

	Packet* GetPacket()
	{
		return m_packet;
	}
private:
	Packet *m_packet;
};

#endif/*NET_PACKET_H*/