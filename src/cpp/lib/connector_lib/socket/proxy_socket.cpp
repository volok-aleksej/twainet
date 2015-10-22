#ifdef WIN32
#	define WIN32_LEAN_AND_MEAN
#	include <winsock2.h>
#	include <windows.h>
#else
#	include <sys/types.h>
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <netdb.h>
#	include <stdio.h>

#	define sprintf_s snprintf

#endif/*WIN32*/

#include "proxy_socket.h"
#include "utils/base64.h"
#include "utils/utils.h"
#include "md5.h"
#include "ntlm.h"
#include <vector>

#define HTTP_RESPONSE_OK 200
#define HTTP_RESPONSE_PROXY_AUTH_REQ 407
#define NTLM_BUF_SIZE	512

#ifdef WIN32

g_dumpSmbNtlmAuthRequest dumpSmbNtlmAuthRequest;
g_dumpSmbNtlmAuthChallenge dumpSmbNtlmAuthChallenge;
g_dumpSmbNtlmAuthResponse dumpSmbNtlmAuthResponse;
g_buildSmbNtlmAuthRequest buildSmbNtlmAuthRequest;
g_buildSmbNtlmAuthRequest_noatsplit buildSmbNtlmAuthRequest_noatsplit;
g_buildSmbNtlmAuthResponse buildSmbNtlmAuthResponse;
g_buildSmbNtlmAuthResponse_noatsplit buildSmbNtlmAuthResponse_noatsplit;
g_ntlm_smb_encrypt ntlm_smb_encrypt;
g_ntlm_smb_nt_encrypt ntlm_smb_nt_encrypt;
g_ntlm_check_version ntlm_check_version;

class LibNtlmInitializer
{
public:
	LibNtlmInitializer()
	{
		HMODULE module = LoadLibraryA("LibNtlm.dll");
		if(!module)
		{
			MessageBoxA(NULL, "Dynamic library LibNtlm.dll isn't found", "Load Error", MB_OK | MB_ICONERROR);
			exit(0);
		}

		dumpSmbNtlmAuthRequest = (g_dumpSmbNtlmAuthRequest)GetProcAddress(module, "dumpSmbNtlmAuthRequest");
		dumpSmbNtlmAuthChallenge = (g_dumpSmbNtlmAuthChallenge)GetProcAddress(module, "dumpSmbNtlmAuthChallenge");
		dumpSmbNtlmAuthResponse = (g_dumpSmbNtlmAuthResponse)GetProcAddress(module, "dumpSmbNtlmAuthResponse");
		buildSmbNtlmAuthRequest = (g_buildSmbNtlmAuthRequest)GetProcAddress(module, "buildSmbNtlmAuthRequest");
		buildSmbNtlmAuthRequest_noatsplit = (g_buildSmbNtlmAuthRequest_noatsplit)GetProcAddress(module, "buildSmbNtlmAuthRequest_noatsplit");
		buildSmbNtlmAuthResponse = (g_buildSmbNtlmAuthResponse)GetProcAddress(module, "buildSmbNtlmAuthResponse");
		buildSmbNtlmAuthResponse_noatsplit = (g_buildSmbNtlmAuthResponse_noatsplit)GetProcAddress(module, "buildSmbNtlmAuthResponse_noatsplit");
		ntlm_smb_encrypt = (g_ntlm_smb_encrypt)GetProcAddress(module, "ntlm_smb_encrypt");
		ntlm_smb_nt_encrypt = (g_ntlm_smb_nt_encrypt)GetProcAddress(module, "ntlm_smb_nt_encrypt");
		ntlm_check_version = (g_ntlm_check_version)GetProcAddress(module, "ntlm_check_version");
	}
};

LibNtlmInitializer libNtlminitializer;
#endif/*WIN32*/


const std::string ProxySocket::auth_header_BASIC = "Proxy-Authenticate: Basic";
const std::string ProxySocket::auth_header_DIGEST = "Proxy-Authenticate: Digest";
const std::string ProxySocket::auth_header_NTLM = "Proxy-Authenticate: NTLM";
const std::string ProxySocket::auth_header_Negotiate = "Proxy-Authenticate: Negotiate";

ProxySocket::ProxySocket(const std::string& ip, int port)
	: m_ip(ip), m_port(port) {}

void ProxySocket::SetUserName(const std::string& userName)
{
	m_userName = userName;
}

void ProxySocket::SetPassword(const std::string& password)
{
	m_password = password;
}

bool ProxySocket::PerformProxyConnection(const std::string& ip, int port)
{
	m_targetIp = ip;
	m_targetPort = port;


	if(!sendConnectCmd(generateStandartHeaders()))
	{
		return false;
	}

	std::string response;
	int responseCode;
	if(!recvProxyResponse(responseCode, response))
	{
		return false;
	}

	if(responseCode == HTTP_RESPONSE_OK)
	{
		return true;
	}
	else if(responseCode == HTTP_RESPONSE_PROXY_AUTH_REQ)
	{
		return performProxyAuthtorization(response);
	}

	return false;
}
	
bool ProxySocket::sendConnectCmd(const std::string& httpheaders)
{
	char portBuffer[6] = {0};
	sprintf_s(portBuffer, 6, "%d", m_targetPort);
	std::string resultString("CONNECT ");
	resultString += m_targetIp + ":" + portBuffer + " HTTP/1.1\r\n" + httpheaders + "\r\n";
	return SendData((char*)resultString.c_str(), (int)resultString.size());
}

bool ProxySocket::recvProxyResponse(int& responseCode, std::string& proxyResponse)
{
	char http_header[10] = {0};
	if(!RecvData(http_header, 9))
	{
		return false;
	}

	proxyResponse.append(http_header);

	char responce_code[4] = {0};
	if(!RecvData(responce_code, 3))
	{
		return false;
	}

	proxyResponse.append(responce_code);

	responseCode = atoi(responce_code);

	int dataLen = 0;
	std::string line;
	char simbol;
	while(true)
	{
		if(!RecvData(&simbol, 1))
		{
			return false;
		}

		if(simbol != '\n')
		{
			line.push_back(simbol);
		}
		else if(line.empty() || line == "\r")
		{
			break;
		}
		else if(line.find("Content-Length:") != -1)
		{
			std::string lenghtData(line.begin() + strlen("Content-Length:"), line.end());
			while(true)
			{
				size_t pos = lenghtData.find(" ");
				if(pos == -1)
				{
					break;
				}
				lenghtData.erase(lenghtData.begin() + pos);
			}

			dataLen = atoi(lenghtData.c_str());
			proxyResponse.append(line);
			proxyResponse.append("\n");
			line.clear();
		}
		else
		{
			proxyResponse.append(line);
			proxyResponse.append("\n");
			line.clear();
		}
	}

	std::string data(dataLen, 0);
	if(!RecvData((char*)data.c_str(), dataLen))
	{
		return false;
	}

	proxyResponse.append(data);

	return true;
}

bool ProxySocket::performProxyAuthtorization(const std::string& proxyResponse)
{
	if(!ReConnect())
	{
		return false;
	}

	bool res = false;
	if (std::string::npos != proxyResponse.find(auth_header_NTLM) ||
		std::string::npos != proxyResponse.find(auth_header_Negotiate))
	{
		res = performNtlmProxyAuthentication(proxyResponse);
	}
	else if (std::string::npos != proxyResponse.find(auth_header_DIGEST) )
	{
		res = performDigestProxyAuthentication(proxyResponse);
	}
	else if (std::string::npos != proxyResponse.find(auth_header_BASIC) )
	{
		res = performBasicProxyAuthentication(proxyResponse);
	}

	return res;
}

bool ProxySocket::performBasicProxyAuthentication(const std::string& proxyResponse)
{
	std::string basicAuthHeader;
	basicAuthHeader.assign("Proxy-authorization: ").append("Basic").append(" ");

	std::string userAndPwd;
	userAndPwd.assign(m_userName).append(":").append(m_password);

	std::string userAndPwdEncoded = base64_encode((const unsigned char*)userAndPwd.c_str(), (unsigned int)userAndPwd.size());

	basicAuthHeader += userAndPwdEncoded + "\r\n" + generateStandartHeaders();

	if (!sendConnectCmd(basicAuthHeader))
	{
		return false;
	}
	
	std::string response;
	int responseCode;
	if(!recvProxyResponse(responseCode, response))
	{
		return false;
	}

	if (HTTP_RESPONSE_OK == responseCode) //got successfully established connection
	{
		return true;
	}

	return false;
}

bool ProxySocket::performDigestProxyAuthentication(const std::string& proxyResponse)
{
	size_t posBegin = proxyResponse.find(auth_header_DIGEST) + auth_header_DIGEST.size();
	size_t posEnd = proxyResponse.find("\r\n", posBegin);
	if(posBegin == -1 || posEnd == -1)
	{
		return false;
	}

	std::string proxyAuthData(proxyResponse.begin() + posBegin, proxyResponse.begin() + posEnd);
	std::vector<std::string> digestParams = CommonUtils::DelimitString(proxyAuthData, ",");
	std::string digestAuthTypeStr, realm, domain, nonce, algorithm, opaque;
	std::string cnonce = "13ed7a1452379929"; /* use static client counter... shall work!*/
	std::string nonceCount = "00000001";
	
	struct DigestType
	{
		char name[10];
		std::string* var;
	} digestTypes[6] = {{"realm", &realm}, {"domain", &domain}, {"nonce", &nonce},
	{"algorithm", &algorithm}, {"opaque", &opaque}, {"qop", &digestAuthTypeStr}};

	for(std::vector<std::string>::iterator it = digestParams.begin(); it != digestParams.end(); it++)
	{
		std::vector<std::string> nameValue = CommonUtils::DelimitString(*it, "=");
		if(nameValue.size() != 2)
		{
			return false;
		}

		for(int i = 0; i < 6; i++)
		{
			if(nameValue[0].find(digestTypes[i].name) != -1)
			{
				if(nameValue[1].find("\"") != -1)
				{
					nameValue[1].erase(nameValue[1].begin());
					nameValue[1].erase(nameValue[1].end() - 1);
				}

				*digestTypes[i].var = nameValue[1];
			}
		}
	}

	char authType = getDigestAuthType(digestAuthTypeStr);
	std::string forEncoding;
	forEncoding.assign(m_userName).append(":").append(realm).append(":").append(m_password);
	std::string ha1 = encryptToMd5(forEncoding);
	std::string response;
	
	std::string remoteAddress(m_targetIp);
	remoteAddress.append(":");
	char port[10];
	sprintf_s(port, 10, "%d", m_targetPort);
	remoteAddress.append(port);
	if (DIGEST_AUTH & authType)
	{
		forEncoding.assign("CONNECT").append(":").append(remoteAddress);
		std::string ha2 = encryptToMd5(forEncoding);

		forEncoding.assign(ha1).append(":").append(nonce).append(":").append(nonceCount).append(":").append(cnonce).append(":").append(digestAuthTypeStr).append(":").append(ha2);
		response = encryptToMd5(forEncoding);
	}
	else if (DIGEST_AUTH_INTEGRITY & authType)
	{
		return false; /* not realized yet */
	}
	else if (DIGEST_AUTH_UNKNOWN & authType)
	{
		forEncoding.assign("CONNECT").append(":").append(remoteAddress);
		std::string ha2 = encryptToMd5(forEncoding);

		forEncoding.assign(ha1).append(":").append(nonce).append(":").append(ha2);
		response = encryptToMd5(forEncoding);
	}

	std::string resultString;
	resultString.assign("Proxy-authorization: ").append("Digest").append(" ");
	resultString.append("username=\"").append(m_userName).append("\", ");
	resultString.append("realm=\"").append(realm).append("\", ");
	resultString.append("qop=").append("auth").append(", "); //force to use auth?
	resultString.append("nc=").append(nonceCount).append(", ");
	resultString.append("cnonce=").append(cnonce).append(", ");
	resultString.append("nonce=").append(nonce).append(", ");
	resultString.append("uri=").append(remoteAddress).append(", ");
	resultString.append("response=\"").append(response).append("\"");

	if (opaque.length() > 0)
	{
		resultString.append(", opaque=\"").append(opaque).append("\"");
	}

	resultString.append("\r\n").append(generateStandartHeaders());
	
	if (!sendConnectCmd(resultString))
	{
		return false;
	}

	int responseCode;
	if(!recvProxyResponse(responseCode, response))
	{
		return false;
	}

	if(responseCode == HTTP_RESPONSE_OK)
	{
		return true;
	}

	return false;
}

bool ProxySocket::performNtlmProxyAuthentication(const std::string& proxyResponse)
{
	tSmbNtlmAuthRequest request;
	buildSmbNtlmAuthRequest( &request, m_userName.c_str(), "");
	char buffer[NTLM_BUF_SIZE] = {0};
	int n = 0;
	if ((n = base64_ntop((unsigned char*)&request, SmbLength(&request), buffer, NTLM_BUF_SIZE)) <= 0)
	{
		return false;
	}

	buffer[n] = '\0';
	std::string resultString;
	resultString = resultString + "Proxy-authorization: NTLM " + buffer + "\r\n" + generateStandartHeaders();
	if(!sendConnectCmd(resultString))
	{
		return false;
	}

	int codeResponse;
	std::string rsp;
	if(!recvProxyResponse(codeResponse, rsp))
	{
		return false;
	}

	size_t posBegin = std::string::npos, posEnd = std::string::npos;
	size_t pos = rsp.find(auth_header_NTLM);
	if (std::string::npos != pos)
	{
		posBegin = pos + auth_header_NTLM.size();
	}

	pos = rsp.find(auth_header_Negotiate);
	if (std::string::npos != pos)
	{
		posBegin = pos + auth_header_Negotiate.size();
	}

	posEnd = rsp.find("\r\n", posBegin);
	if(posEnd == std::string::npos || posEnd - posBegin > NTLM_BUF_SIZE)
	{
		return false;
	}

	memset(buffer, 0, NTLM_BUF_SIZE);
	memcpy(buffer, rsp.c_str() + posBegin, posEnd - posBegin);
	tSmbNtlmAuthChallenge challenge;
	if(base64_pton(buffer, (unsigned char*)&challenge, sizeof(challenge)) <= 0)
	{
		return false;
	}
	
	tSmbNtlmAuthResponse response;
	buildSmbNtlmAuthResponse(&challenge, &response, m_userName.c_str(), m_password.c_str());
	if ((n = base64_ntop( ( unsigned char * ) &response, SmbLength(&response), buffer, NTLM_BUF_SIZE)) <= 0)
	{
		return false;
	}
	
	buffer[n] = '\0';
	resultString.clear();
	resultString = resultString + "Proxy-authorization: NTLM " + buffer + "\r\n" + generateStandartHeaders();
	if(!sendConnectCmd(resultString))
	{
		return false;
	}

	rsp.clear();
	if(!recvProxyResponse(codeResponse, rsp))
	{
		return false;
	}

	if(codeResponse == HTTP_RESPONSE_OK)
	{
		return true;
	}

	return false;
}

char ProxySocket::getDigestAuthType(const std::string &authString)
{
   char retVal = DIGEST_AUTH_UNKNOWN;

   size_t authIntPos = authString.find("auth-in");
   if (std::string::npos != authIntPos)
   {
      retVal &= ~DIGEST_AUTH_UNKNOWN; //clear DIGEST_AUTH_UNKNOWN flag
      retVal |= DIGEST_AUTH_INTEGRITY;
   }

   size_t authPos = 0;
   
   for (;;)
   {
      authPos = authString.find("auth", authPos);
      if (std::string::npos == authPos)
      {
         break;
      }

      if (authPos != authIntPos)
      {
         retVal &= ~DIGEST_AUTH_UNKNOWN; //clear DIGEST_AUTH_UNKNOWN flag
         retVal |= DIGEST_AUTH;
         break;
      }

      ++authPos;
   }


   return retVal;
}
const std::string ProxySocket::encryptToMd5(const std::string &sourceString)
{
   md5_state_s pms;
   md5_byte_t digest[16];

   /* Initialize the algorithm. */
   md5_init(&pms);

   /* Append a string to the message. */
   md5_append(&pms
            , reinterpret_cast<const md5_byte_t*> (sourceString.c_str())
            , (unsigned int) sourceString.length());

   /* Finish the message and return the digest. */
   md5_finish(&pms, digest);

   char hex_output[16*2 + 1];
   for (int di = 0; di < 16; ++di)
   {
      sprintf_s(hex_output + di * 2, 16*2 + 1 - di * 2, "%02x", digest[di]);
   }

   std::string resultString (hex_output, 32);

   return resultString;
}

const std::string ProxySocket::generateStandartHeaders()
{
	char portBuffer[6] = {0};
	sprintf_s(portBuffer, 6, "%d", m_targetPort);
	std::string result;
	result.append("Proxy-Connection: Keep-Alive\r\n");
	result.append("Host: ").append(m_targetIp).append(":").append(portBuffer).append("\r\n");
	return result;
}

/*********************************************************************************/
/*                               ProxyTCPSocket                                  */
/*********************************************************************************/
ProxyTCPSocket::ProxyTCPSocket(const std::string& ip, int port, IPVersion ipv)
	: ProxySocket(ip, port), TCPSocket(ipv){}

ProxyTCPSocket::ProxyTCPSocket(int socket, const std::string& ip, int port)
	: ProxySocket(ip, port), TCPSocket(socket) {}

bool ProxyTCPSocket::Connect(const std::string& ip, int port)
{
	if(!TCPSocket::Connect(m_ip, m_port))
	{
		return false;
	}

	return PerformProxyConnection(ip, port);
}

bool ProxyTCPSocket::SendData(char* data, int len)
{
	return TCPSocket::Send(data, len);
}

bool ProxyTCPSocket::RecvData(char* data, int len)
{
	return TCPSocket::Recv(data, len);
}

bool ProxyTCPSocket::ReConnect()
{
	Close();
	Initialize();
	Bind("", 0);
	return TCPSocket::Connect(m_ip, m_port);
}

/*********************************************************************************/
/*                               SecureProxyTCPSocket                            */
/*********************************************************************************/
SecureProxyTCPSocket::SecureProxyTCPSocket(const std::string& ip, int port, IPVersion ipv)
	: ProxySocket(ip, port), SecureTCPSocket(ipv) {}

SecureProxyTCPSocket::SecureProxyTCPSocket(int socket, const std::string& ip, int port)
	: ProxySocket(ip, port), SecureTCPSocket(socket){}

bool SecureProxyTCPSocket::Connect(const std::string& ip, int port)
{
	if(!TCPSocket::Connect(m_ip, m_port) ||
		!PerformProxyConnection(ip, port))
	{
		return false;
	}

	return PerformSslVerify();
}

bool SecureProxyTCPSocket::SendData(char* data, int len)
{
	return TCPSocket::Send(data, len);
}

bool SecureProxyTCPSocket::RecvData(char* data, int len)
{
	return TCPSocket::Recv(data, len);
}

bool SecureProxyTCPSocket::ReConnect()
{
	Close();
	Initialize();
	Bind("", 0);
	return TCPSocket::Connect(m_ip, m_port);
}