#include "internal_connector.h"

InternalConnector::InternalConnector(AnySocket* socket)
: Connector(socket), m_maxBufferSize(0)
{
}

InternalConnector::~InternalConnector()
{
	removeReceiver();
}

void InternalConnector::SubscribeConnector(::SignalOwner* owner)
{
	owner->addSubscriber(this, SIGNAL_FUNC(this, InternalConnector, InternalConnectionDataMessage, onInternalConnectionDataMessage));
}
	
void InternalConnector::ThreadFunc()
{
	int sizedata = m_socket->GetMaxBufferSize();
  	char *data = new char[sizedata];
	while(!IsStop())
	{
		int res = recv(m_socket->GetSocket(), data, sizedata, 0);
		if (!res)
		{
			break;
		}

		InternalConnectionData icd;
		icd.set_id(GetId());
		icd.set_data(data, res);
		InternalConnectionDataSignal icdSig(icd);
		onSignal(icdSig);
	}
	
	delete data;
}

void InternalConnector::OnStart()
{
}

void InternalConnector::OnStop()
{
}

void InternalConnector::onInternalConnectionDataMessage(const InternalConnectionDataMessage& msg)
{
	if(msg.id() == GetId())
	{
		if(!m_socket->Send((char*)msg.data().c_str(), msg.data().size()))
		{
			Stop();
		}
	}
}