#include "tcp_socket.h"

#include <IPAddress.h>
#include <debug.h>
#include <Arduino.h>
#include <lwip/dns.h>
#include <lwip/err.h>

extern "C" void esp_yield();
extern "C" void esp_schedule();

void onError(void* arg, int8_t err)
{
    TCPSocket* socket = reinterpret_cast<TCPSocket*>(arg);
    if(socket) {
        socket->OnError(err);
    }
}

void dnsFoundCallback(const char *name, ip_addr_t *ipaddr, void *callback_arg)
{
    if(ipaddr) {
        (*reinterpret_cast<IPAddress*>(callback_arg)) = ipaddr->addr;
    }
    esp_schedule(); // resume the hostByName function
}

int8_t onTcpAccept(void *arg, tcp_pcb* newpcb, int8_t err)
{
    TCPSocket* socket = reinterpret_cast<TCPSocket*>(arg);
    if(socket) {
        return socket->OnAccept(newpcb, err);
    }
    
    return ERR_VAL;
}

int8_t onTcpConnect(void* arg, tcp_pcb* tpcb, int8_t err)
{
    TCPSocket* socket = reinterpret_cast<TCPSocket*>(arg);
    if(socket) {
        return socket->OnConnect(tpcb, err);
    }
    
    return ERR_VAL;
}

int8_t onTCPRecv(void *arg, struct tcp_pcb *tpcb, struct pbuf *pb, err_t err)
{
    TCPSocket* socket = reinterpret_cast<TCPSocket*>(arg);
    if(socket) {
        return socket->OnTCPRecv(tpcb, pb, err);
    }
    
    return ERR_VAL;
}

int8_t onTCPSent(void *arg, struct tcp_pcb *tpcb, uint16_t len)
{
    TCPSocket* socket = reinterpret_cast<TCPSocket*>(arg);
    if(socket) {
        return socket->OnTCPSent(tpcb, len);
    }
    
    return ERR_VAL;
}

TCPSocket::TCPSocket()
: m_listenSocket(INVALID_SOCKET)
, m_acceptedSocket(INVALID_SOCKET)
, m_buf_offset(0)
, m_buf(0)
, m_sentSize(0)
{
	Initialize();
}

TCPSocket::TCPSocket(int socket)
: m_socket(*(tcp_pcb**)&socket)
, m_listenSocket(INVALID_SOCKET)
, m_acceptedSocket(INVALID_SOCKET)
, m_buf_offset(0)
, m_buf(0)
, m_sentSize(0)
{
    tcp_arg(m_socket, this);
}

TCPSocket::~TCPSocket()
{
	Close();
}

bool TCPSocket::Bind(const String& host, int port)
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

    ip_addr_t ipaddr;
	if(host.length() == 0)
	{
        ipaddr = ip_addr_any;
	}
	else
	{
        IPAddress ipaddress;
        if(!ipaddress.fromString(host)) {
            err_t err = dns_gethostbyname(host.c_str(), &ipaddr, &dnsFoundCallback, &ipaddress);
            if(err == ERR_INPROGRESS) {
                esp_yield();
                // will return here when dns_found_callback fires
                ipaddr.addr = ipaddress;
            } else if(err != ERR_OK) {
                return false;
            }
        }
	}
	
	return tcp_bind(m_socket, &ipaddr, port) == ERR_OK;
}

bool TCPSocket::Listen(int limit)
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

	return true;
}

int TCPSocket::Accept(String& ip, int& port)
{
	if(m_socket == INVALID_SOCKET)
	{
		return INVALID_SOCKET;
	}

	m_listenSocket = tcp_listen(m_socket);
    if(m_listenSocket) {
        tcp_accept(m_listenSocket, &onTcpAccept);
        tcp_arg(m_listenSocket, this);
        esp_yield();
    }
    
    int ret = *(int*)m_acceptedSocket;
    m_acceptedSocket = INVALID_SOCKET;
    IPAddress address(m_acceptedSocket->remote_ip.addr);
    ip = address.toString();
    port = m_acceptedSocket->remote_port;
	return ret;
}

bool TCPSocket::Connect(const String& host, int port)
{
	if(m_socket == INVALID_SOCKET || host.length() == 0)
	{
		return false;
	}

    ip_addr_t ipaddr;
    if(host.length() == 0)
    {
        ipaddr = ip_addr_any;
    }
    else
    {
        IPAddress ipaddress;
        if(!ipaddress.fromString(host)) {
            err_t err = dns_gethostbyname(host.c_str(), &ipaddr, &dnsFoundCallback, &ipaddress);
            if(err == ERR_INPROGRESS) {
                esp_yield();
                // will return here when dns_found_callback fires
                ipaddr.addr = ipaddress;
            } else if(err != ERR_OK) {
                return false;
            }
        }
    }
    
	err_t err = tcp_connect(m_socket, &ipaddr, port, &onTcpConnect);
    esp_yield();
    return err == ERR_OK;
}

bool TCPSocket::Send(char* data, int len)
{
	if(m_socket == INVALID_SOCKET || m_sentSize)
	{
		return false;
	}
	
 	m_sentSize = len;
    while(m_sentSize) {
        size_t room = tcp_sndbuf(m_socket);
        size_t will_send = (room < m_sentSize) ? room : m_sentSize;
        err_t err = tcp_write(m_socket, data + len - m_sentSize, will_send, 0);
        if(err != ERR_OK) {
            DEBUGV("TCPSocket:Send !ERR_OK\r\n");
            return false;
        }
        
        tcp_output(m_socket);
        esp_yield();
    }
	
	return true;
}

bool TCPSocket::Recv(char* data, int len)
{
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}
    
    if(!m_buf) {
        esp_yield();
    }

    
    DEBUGV(":rd %d, %d, %d\r\n", len, m_buf->tot_len, m_buf_offset);
    while(len) {
        size_t max_size = m_buf->tot_len - m_buf_offset;
        size_t size = (len < max_size) ? len : max_size;
        size_t buf_size = m_buf->len - m_buf_offset;
        size_t copy_size = (max_size < buf_size) ? max_size : buf_size;
        DEBUGV(":rdi %d, %d\r\n", buf_size, copy_size);
        os_memcpy(data, reinterpret_cast<char*>(m_buf->payload) + m_buf_offset, copy_size);
        data += copy_size;
        Consume(copy_size);
        len -= copy_size;
    }
    
	return false;
}
void TCPSocket::Consume(size_t size)
{
    ptrdiff_t left = m_buf->len - m_buf_offset - size;
    if(left > 0) {
        m_buf_offset += size;
    } else if(!m_buf->next) {
        DEBUGV(":c0 %d, %d\r\n", size, _rx_buf->tot_len);
        if(m_socket) tcp_recved(m_socket, m_buf->len);
        pbuf_free(m_buf);
        m_buf = 0;
        m_buf_offset = 0;
    } else {
        DEBUGV(":c %d, %d, %d\r\n", size, _rx_buf->len, _rx_buf->tot_len);
        auto head = m_buf;
        m_buf = m_buf->next;
        m_buf_offset = 0;
        pbuf_ref(m_buf);
        if(m_socket) tcp_recved(m_socket, head->len);
        pbuf_free(head);
    }
}

bool TCPSocket::Close()
{
    if(m_listenSocket != INVALID_SOCKET)
    {
        tcp_close(m_listenSocket);
    }
    
    if(m_acceptedSocket != INVALID_SOCKET)
    {
        tcp_close(m_acceptedSocket);
    }
    
	if(m_socket == INVALID_SOCKET)
	{
		return false;
	}

	return tcp_close(m_socket) == ERR_OK;
}

void TCPSocket::GetIPPort(String& ip, int& port)
{
    IPAddress address(m_acceptedSocket->remote_ip.addr);
    ip = address.toString();
    port = m_acceptedSocket->remote_port;
}

int TCPSocket::GetSocket()
{
	return *(int*)m_socket;
}

void TCPSocket::Initialize()
{
	m_socket = tcp_new();
    tcp_arg(m_socket, this);
    tcp_err(m_socket, &onError);
}

int TCPSocket::GetMaxBufferSize()
{
	if(m_socket == INVALID_SOCKET)
	{
		return 0;
	}
	
	return m_socket->snd_buf;
}

void TCPSocket::OnError(uint8_t err)
{
    DEBUGV("TCPSocket:error:%d\r\n", err);
}

int8_t TCPSocket::OnAccept(tcp_pcb* newpcb, int8_t err)
{
    DEBUGV("TCPSocket:accept\r\n");
    m_acceptedSocket = newpcb;
    tcp_accepted(m_listenSocket);
    m_listenSocket = INVALID_SOCKET;
    tcp_setprio(m_acceptedSocket, TCP_PRIO_MIN);
    tcp_recv(m_acceptedSocket, (tcp_recv_fn)&onTCPRecv);
    tcp_sent(m_acceptedSocket, &onTCPSent);
    tcp_err(m_acceptedSocket, &onError);
    esp_schedule();
    return ERR_OK;
}

int8_t TCPSocket::OnConnect(tcp_pcb* tpcb, int8_t err)
{
    DEBUGV("TCPSocket:connected\r\n");
    m_socket = tpcb;
    tcp_setprio(m_socket, TCP_PRIO_MIN);
    tcp_arg(m_socket, this);
    tcp_recv(m_socket, (tcp_recv_fn)&onTCPRecv);
    tcp_sent(m_socket, &onTCPSent);
    tcp_err(m_socket, &onError);        
    esp_schedule();
    return ERR_OK;
}

int8_t TCPSocket::OnTCPRecv(tcp_pcb* tpcb, pbuf* pb, err_t err)
{
    if(m_buf) {
        pbuf_cat(m_buf, pb);
    } else {
        m_buf = pb;
    }
    return ERR_OK;
}

int8_t TCPSocket::OnTCPSent(tcp_pcb* tpcb, uint16_t len)
{
    m_sentSize -= len;
    esp_schedule();
    return ERR_OK;
}