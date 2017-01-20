#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <WString.h>
#include "include/any_socket.h"
#include "message_handler.h"
#include "thread.h"

class Connector : public MessageHandler, public Thread
{
public:
	Connector(AnySocket* socket);
	virtual ~Connector();

	void SetId(const String& id);
	String GetId() const;
	void SetRemoteAddr(const String& ip, int port);
	void SetConnectorId(const String& connectorId);
	String GetConnectorId();

	virtual void Stop();
    virtual void onNewConnector(const Connector* connector){};
protected:
	virtual bool SendData(char* data, int len);
protected:
	AnySocket* m_socket;
	String m_id;
	String m_connectorId;

    String m_remoteIp;
	int m_remotePort;
};

#endif/*CONNECTOR_H*/