#include "secure_socket.h"
#include "any_socket.h"

SecureSocket::SecureSocket(AnySocket* socket)
{
	initSSL();
}

SecureSocket::~SecureSocket()
{
	Shutdown();
}

bool SecureSocket::initSSL()
{
	m_sslctx = SSL_CTX_new(SSLv3_method());
	if (!m_sslctx)
	{
		return false;
	}

	m_ssl = SSL_new(m_sslctx);
	if (!m_ssl)
	{
		return false;
	}

	return true;
}

bool SecureSocket::PerformSslVerify()
{
	if(!m_ssl)
	{
		initSSL();
	}
	
	return true;
}

bool SecureSocket::Send(char* data, int len)
{
	if(!m_ssl)
	{
		initSSL();
	}

	return true;
}

bool SecureSocket::Recv(char* data, int len)
{
	if(!m_ssl)
	{
		initSSL();
	}

	return true;
}

void SecureSocket::Shutdown()
{
   if (m_sslctx)
   {
      SSL_CTX_free(m_sslctx);
      m_sslctx = NULL;
   }

   if (m_ssl) 
   {
      SSL_free(m_ssl);
      m_ssl = NULL;
   }
}
