#ifdef WIN32
	#include <winsock2.h>
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

std::vector<std::string> GetLocalIps()
{
	std::vector<std::string> ips;
#ifdef WIN32
	char chInfo[64];
	if (!gethostname(chInfo, sizeof(chInfo)))
	{
		hostent *sh;
		sh = gethostbyname((char*)&chInfo);
		if (sh != NULL)
		{
			int nAdapter = 0;
			while (sh->h_addr_list[nAdapter])
			{
				struct sockaddr_in addr;
				memcpy(&addr.sin_addr, sh->h_addr_list[nAdapter], sh->h_length);
				nAdapter++;
				ips.push_back(inet_ntoa(addr.sin_addr));
			}
		}
	}
#else
	struct ifaddrs *ifaddr, *ifa;
	int family, s;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1) {
		return;
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;
		family = ifa->ifa_addr->sa_family;
		/* For an AF_INET* interface address, display the address */

		if (family == AF_INET /*|| family == AF_INET6*/) {
			s = getnameinfo(ifa->ifa_addr,
				(family == AF_INET) ? sizeof(struct sockaddr_in) :
				sizeof(struct sockaddr_in6),
				host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			if (s != 0) {
				return;
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