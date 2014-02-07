#ifndef EXTERNAL_CONNECT_THREAD_H
#define EXTERNAL_CONNECT_THREAD_H

#include "connector_lib/thread/connect_thread.h"

class ExternalConnectThread : public ConnectThread
{
public:
	ExternalConnectThread(const ConnectAddress& address);
	virtual ~ExternalConnectThread();

	virtual void Stop();

	void ChangeConnectAddress(const ConnectAddress& address);
protected:
	virtual void OnStart(){}
	virtual void OnStop(){}
	virtual void ThreadFunc();

	void SignalError();

private:
	AnySocket* m_udpSocket;
	bool m_isChangeAddress;
	CriticalSection m_csSocket;
};

#endif/*EXTERNAL_CONNECT_THREAD_H*/