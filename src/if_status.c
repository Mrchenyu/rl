#include <stdio.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

int get_mac(char *iface, unsigned char *mac)
{
    int skfd;
    struct sockaddr_in addrin;
    struct ifreq ifr;

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (skfd == -1)
    {
        perror("socket error");
	    return 1;
    }

    strcpy(ifr.ifr_name, iface);
    ioctl(skfd, SIOCGIFHWADDR, &ifr);

    memcpy(mac, ifr.ifr_hwaddr.sa_data, 6);

    return 0;
}

int get_system_value(char * cmd, char * value)
{
    FILE *fp;
    char buff[256];
    int i = 0;

    fp = popen(cmd, "r");
    if (fp == NULL)
    {
        printf("popen error");
        return 1;
    }

    memset(buff, 0, sizeof(buff));
    fgets(buff, sizeof(buff), fp);

    for (i = 0; buff[i] != 0; i++)
    {
        if (buff[i] == '\n')
            buff[i] = 0;
    }

    strcpy(value, buff);
    pclose(fp);

    return 0;
}

int get_if_ip(char *iface, char *ip)
{
	struct ifreq ifr;
	int fd = 0;

	memset(&ifr, 0, sizeof(struct ifreq));

	fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	strcpy(ifr.ifr_name, iface);
	if (ioctl(fd, SIOCGIFADDR, &ifr) == 0)
	{
		struct sockaddr_in *x = (struct sockaddr_in *) &ifr.ifr_addr;
		strcpy(ip, inet_ntoa(x->sin_addr));
		return 1;
	}
	else
		return 0;
}
