#ifndef NET_PACKET_H
#define NET_PACKET_H

#include <vector>
#include <string>

class IConnection;

class IConnectionPacket
{
public:
	virtual ~IConnectionPacket(){}

	virtual bool IsConnectionPacket(IConnection* conn) = 0;
	virtual void PacketConnection(IConnection* conn) = 0;
	virtual std::string GetType() = 0;
	virtual IConnectionPacket* Clone() = 0;
};

class IConnection
{
public:
	virtual ~IConnection(){}

	virtual bool IsConnectionPacket(IConnectionPacket* packet)
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

template<typename Packet, typename Connection>
class ConnectionPacket : public IConnectionPacket
{
public:
	ConnectionPacket() : m_packet(0){}
	ConnectionPacket(Packet* packet)
		: m_packet(packet){}
	ConnectionPacket(const ConnectionPacket* packet)
		: m_packet(dynamic_cast<Packet*>(packet->m_packet->Clone()))
	{
	}
	virtual ~ConnectionPacket()
	{
		if(m_packet)
			delete m_packet;
	}

	virtual bool IsConnectionPacket(IConnection* conn)
	{
		if(conn)
		{
			return conn->IsConnectionPacket(this);
		}
		return false;
	}

	virtual void PacketConnection(IConnection* conn)
	{
		if(!conn)
			return;
		Connection* connection = dynamic_cast<Connection*>(conn);
		if(connection)
		{
			connection->OnPacket(m_packet);
		}
	}

	virtual std::string GetType()
	{
		return typeid(Packet).name();
	}

	virtual IConnectionPacket* Clone()
	{
		return new ConnectionPacket<Packet, Connection>(this);
	}

private:
	Packet *m_packet;
};

#endif/*NET_PACKET_H*/