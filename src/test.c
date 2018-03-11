#include <stdio.h>
#include <string.h>
#include "tcrule.h"
#include "if_status.h"

#define PRINT 1

int main(int argc, char *argv[])
{
    char cmd[256];
    char iface[256];
    char buff[256];
    unsigned char mac[256];
    int count = 0;
    int offset = 0;
    
    memset(cmd, 0, sizeof(cmd));
    memset(iface, 0, sizeof(iface));
    memset(mac, 0, sizeof(mac));
    memset(buff, 0, sizeof(buff));
    sprintf(cmd, "ifconfig | grep flags | sed -n '1p' | cut -d : -f 1 2>/dev/null");
    get_system_value(cmd, buff);
    
    strcpy(iface, buff); /*get interface*/
    
    get_mac(buff, mac);
    tcrule_init(iface, 50);
#if PRINT
    printf("iface:%s,mac:%02x-%02x-%02x-%02x-%02x-%02x\n", iface, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
#endif
    count ++;
    
    tc_class_add(iface, count, 20);
    tc_filter_add_on_mac(iface, mac + offset, count);

    offset += offset + count * 6;
    memset(cmd, 0, sizeof(cmd));
    memset(buff, 0, sizeof(buff));
    
    /*for this interface just to get mac address so don't init*/
    sprintf(cmd, "ifconfig | grep flags | sed -n '2p' | cut -d : -f 1 2>/dev/null");
    get_system_value(cmd, buff);
    get_mac(buff, mac + offset);
#if PRINT
    printf("iface:%s,mac:%02x-%02x-%02x-%02x-%02x-%02x\n", buff, (mac + offset)[0], (mac + offset)[1], \
    (mac + offset)[2], (mac + offset)[3], (mac + offset)[4], (mac + offset)[5]);
#endif
    count ++;

    tc_class_add(iface, count, 30);
    tc_filter_add_on_mac(iface, mac + offset, count);

    offset += offset + count * 6;
    tc_show(iface);

    tcrule_del(iface, mac);
    return 0;
}
