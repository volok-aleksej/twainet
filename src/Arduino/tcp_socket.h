#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include "any_socket.h"

#include <WString.h>
#include <lwip/netif.h>
#include <lwip/tcp.h>

class TCPSocket : public AnySocket
{
public:
	TCPSocket();
	explicit TCPSocket(int socket);
	virtual ~TCPSocket();

	virtual bool Bind(const String& host, int port);
	virtual bool Listen(int limit);
	virtual int Accept(String& ip, int& port);
	virtual bool Connect(const String& host, int port);
	virtual bool Send(char* data, int len);
	virtual bool Recv(char* data, int len);
	virtual void GetIPPort(String& ip, int& port);
	virtual bool Close();
	virtual int GetSocket();
	virtual int GetMaxBufferSize();
protected:
	void Initialize();
    void Consume(size_t size);
protected:
    friend void onError(void* arg, int8_t err);
    friend int8_t onTcpAccept(void *arg, tcp_pcb* newpcb, int8_t err);
    friend int8_t onTcpConnect(void* arg, tcp_pcb* tpcb, int8_t err);
    friend int8_t onTCPRecv(void *arg, struct tcp_pcb *tpcb, struct pbuf *pb, err_t err);
    friend int8_t onTCPSent(void *arg, struct tcp_pcb *tpcb, uint16_t len);
    void OnError(uint8_t err);
    int8_t OnAccept(tcp_pcb* newpcb, int8_t err);
    int8_t OnConnect(tcp_pcb* tpcb, int8_t err);
    int8_t OnTCPRecv(tcp_pcb* tpcb, pbuf* pb, err_t err);
    int8_t OnTCPSent(tcp_pcb* tpcb, uint16_t len);
private:
	tcp_pcb* m_socket;
    tcp_pcb* m_acceptedSocket;
    tcp_pcb* m_listenSocket;
    uint32_t m_sentSize;
    pbuf* m_buf;
    uint32_t m_buf_offset;
};

#endif/*TCP_SOCKET_H*/