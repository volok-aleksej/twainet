#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "../socket/any_socket.h"
#include "../handler/message_handler.h"
#include "../signal/signal_receiver.h"

#include "thread_lib/thread/thread_impl.h"

class Connector : public MessageHandler, public ThreadImpl
{
public:
	Connector(AnySocket* socket);
	virtual ~Connector();

	void SetId(const std::string& id);
	std::string GetId()const;
	void SetRemoteAddr(const std::string& ip, int port);
	void SetConnectorId(const std::string& connectorId);
	std::string GetConnectorId();
	void SetAccessId(const std::string& accessId);
	std::string GetAccessId();

	virtual void Stop();
	virtual void onNewConnector(const Connector* connector){};
protected:
	virtual bool SendData(char* data, int len);
protected:
	AnySocket* m_socket;
	std::string m_id;
	std::string m_connectorId;
	std::string m_accessId;

	std::string m_remoteIp;
	int m_remotePort;
};

#endif/*CONNECTOR_H*/
