#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <winsock2.h>
	#include <WS2tcpip.h>
	#include <Iphlpapi.h>
#else
	#include <netdb.h>
    #include <ifaddrs.h>

	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/param.h>

	#include <sys/ioctl.h>

	#include <sys/socket.h>
	#include <net/if.h>
	#include <netinet/in.h>
	#include <net/if_arp.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <iostream>
	#include <time.h>
	#include <stdlib.h>
	#include <stdio.h>
#endif

#include <vector>
#include <string>

std::vector<std::string> GetLocalIps(int ipv)
{
	std::vector<std::string> ips;
#ifdef WIN32
	DWORD rv, size;
	PIP_ADAPTER_ADDRESSES adapter_addresses, aa;
	PIP_ADAPTER_UNICAST_ADDRESS ua;

	rv = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &size);
	if (rv != ERROR_BUFFER_OVERFLOW)
		return ips;

	adapter_addresses = (PIP_ADAPTER_ADDRESSES)malloc(size);

	rv = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapter_addresses, &size);
	if (rv != ERROR_SUCCESS) {
		free(adapter_addresses);
		return ips;
	}

	for (aa = adapter_addresses; aa != NULL; aa = aa->Next) {
		for (ua = aa->FirstUnicastAddress; ua != NULL; ua = ua->Next) {
			char buf[BUFSIZ] = {0};

			if(ua->Address.lpSockaddr->sa_family == ipv) {
				memset(buf, 0, BUFSIZ);
				getnameinfo(ua->Address.lpSockaddr, ua->Address.iSockaddrLength, buf, sizeof(buf), NULL, 0, NI_NUMERICHOST);
				ips.push_back(buf);
			}
		}
	}

	free(adapter_addresses);
#else
	struct ifaddrs *ifaddr, *ifa;
	int family, s;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1) {
		return ips;
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;
		family = ifa->ifa_addr->sa_family;
		/* For an AF_INET* interface address, display the address */

		if (family == ipv) {
			s = getnameinfo(ifa->ifa_addr,
				(family == AF_INET) ? sizeof(struct sockaddr_in) :
				sizeof(struct sockaddr_in6),
				host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			if (s != 0) {
				return ips;
			}
			else if (host[0] == '1' && host[1] == '2' && host[2] == '7')
			{
				continue;
			}
			ips.push_back(host);
		}
	}
	freeifaddrs(ifaddr);
#endif
	return ips;
}

std::string getMAC(sockaddr_in* addr)
{
	char mac_addr[18] = {0};
#ifdef WIN32
	PIP_ADAPTER_INFO AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	DWORD dwBufLen = sizeof(AdapterInfo);

	if (AdapterInfo && GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW)
		AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);

	if (AdapterInfo && GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR)
	{
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
		do
		{
			IP_ADDRESS_STRING* ipaddrstr = 0;
			for(IP_ADDR_STRING* ipaddr = &pAdapterInfo->IpAddressList;
				ipaddr != 0;
				ipaddr = ipaddr->Next)
			{
				std::string ip = inet_ntoa(addr->sin_addr);
				if(ip == ipaddr->IpAddress.String)
				{
					ipaddrstr = &ipaddr->IpAddress;
					break;
				}
			}

			if(!ipaddrstr)
			{
				pAdapterInfo = pAdapterInfo->Next;  
				continue;
			}

			sprintf_s(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
				pAdapterInfo->Address[0], pAdapterInfo->Address[1],
				pAdapterInfo->Address[2], pAdapterInfo->Address[3],
				pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
			printf("Address: %s, mac: %s\n", pAdapterInfo->IpAddressList.IpAddress.String, mac_addr);
			break;
		}while(pAdapterInfo);                        
	}
	free(AdapterInfo);
#endif
	return mac_addr;
}

std::string getMAC(const std::string& name)
{
	char mac_addr[17];
#ifdef WIN32
	PIP_ADAPTER_INFO AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	DWORD dwBufLen = sizeof(AdapterInfo);

	if (AdapterInfo && GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW)
		AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);

	if (AdapterInfo && GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR)
	{
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
		do
		{
			if(name.find(pAdapterInfo->AdapterName) != -1)
			{
				pAdapterInfo = pAdapterInfo->Next;  
				continue;
			}

			sprintf_s(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
				pAdapterInfo->Address[0], pAdapterInfo->Address[1],
				pAdapterInfo->Address[2], pAdapterInfo->Address[3],
				pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
			printf("Address: %s, mac: %s\n", pAdapterInfo->IpAddressList.IpAddress.String, mac_addr);
			break;
		}while(pAdapterInfo);                        
	}
	free(AdapterInfo);
#endif
	return mac_addr;
}