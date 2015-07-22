#ifndef PROXY_SOCKET_H
#define PROXY_SOCKET_H

#include "secure_socket.h"

class ProxySocket
{
	enum digestAuthType
	{
		DIGEST_AUTH_UNKNOWN    = 1,
		DIGEST_AUTH            = 2,
		DIGEST_AUTH_INTEGRITY  = 8
	};
public:
	ProxySocket(const std::string& ip, int port);

	void SetUserName(const std::string& userName);
	void SetPassword(const std::string& password);

	bool PerformProxyConnection(const std::string& ip, int port);
protected:
	virtual bool SendData(char* data, int len) = 0;
	virtual bool RecvData(char* data, int len) = 0;
	virtual bool ReConnect() = 0;

private:
	bool sendConnectCmd(const std::string& httpheaders);
	bool recvProxyResponse(int& responseCode, std::string& proxyResponse);
	bool performProxyAuthtorization(const std::string& proxyResponse);
	bool performBasicProxyAuthentication(const std::string& proxyResponse);
	bool performDigestProxyAuthentication(const std::string& proxyResponse);
	bool performNtlmProxyAuthentication(const std::string& proxyResponse);
	char getDigestAuthType(const std::string &authString);
	const std::string encryptToMd5(const std::string &sourceString);
	const std::string generateStandartHeaders();

protected:
	std::string m_ip;
	int m_port;
private:
	std::string m_userName;
	std::string m_password;

	std::string m_targetIp;
	int m_targetPort;
};

class ProxyTCPSocket : public ProxySocket, public TCPSocket
{
public:
	ProxyTCPSocket(const std::string& ip, int port);
	ProxyTCPSocket(int socket, const std::string& ip, int port);

	bool Connect(const std::string& ip, int port);

protected:
	virtual bool SendData(char* data, int len);
	virtual bool RecvData(char* data, int len);
	virtual bool ReConnect();
};

class SecureProxyTCPSocket : public ProxySocket, public SecureTCPSocket
{
public:
	SecureProxyTCPSocket(const std::string& ip, int port);
	SecureProxyTCPSocket(int socket, const std::string& ip, int port);

	bool Connect(const std::string& ip, int port);

protected:
	virtual bool SendData(char* data, int len);
	virtual bool RecvData(char* data, int len);
	virtual bool ReConnect();
};

#endif/*PROXY_SOCKET_H*/