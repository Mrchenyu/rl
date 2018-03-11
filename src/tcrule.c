#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#define BUF_LEN 256
#define MTU 1500 /* means:maxinum transmission unit */
#define DEBUG 1

static int tc_command(const char *control, ...)
{
    va_list va;
    char buf[BUF_LEN];
    int offset = 0;

    memset(buf, 0, sizeof(buf));
    va_start(va, control);
    offset = sprintf(buf, "%s", "tc ");
    vsprintf(buf + offset, control, va);
    va_end(va);

#if DEBUG
    printf("tc_command:%s\n", buf);
#endif
    return system(buf);
}

int tcrule_init(char *devname, int rate)
{
    int r2q_num = 0;

    /*
     * r2q associated with MTU when equal to 1500 is best
     */
    r2q_num = rate * 1000000 / MTU;
    /*
     * some module need to require
     */
    system("insmod htb");
    system("insmod ifb numifbs=2");
    system("insmod sfq");
    /*
     * engress queue 
     */
    tc_command("qdisc add dev %s root handle 1: htb r2q %d", devname, r2q_num);

    /*
     *ingress queue use ifb redirect to engress
     */
    tc_command("qdisc add dev %s handle ffff: ingress", devname);
    tc_command("qdisc add dev ifb0 handle 1: htb r2q %d", devname, r2q_num);
    tc_command("filter add dev %s handle ffff: protocol ip u32 match u32 0 0 action mirred engress redirected ifb0");

    return 0;
}

int tc_class_add(char *devname, int index, int rate)
{
    tc_command("class add dev %s parent 1: classid 1:%d htb rate %dmbit burst %dk", devname, index, rate, rate / 10);

    tc_command("class add dev ifb0 parent 1: classid 1:%d htb rate %dmbit burst %dk", index, rate, rate / 10);

    return 0;
}

int tc_filter_add_on_mac(char *devname, char *mac_addr, int index)
{
    /*
     * match destination MAC  0xM2M3M4M5  0xM0M1
     */
    tc_command("filter add dev %s parent 1: protocol ip prio 5 u32 match u16 0x0800\
0xffff at -2  match u32 0x%2x%2x%2x%2x 0xffffffff at -12 u16 0x%2x%2x 0xffff at -14 flowid 1:%d", 
devname, mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5], mac_addr[0], mac_addr[1], index);
    /*
     * arveage each qidsc packets
     */
    tc_command("qdisc add dev %s parent 1:%d handle %d sfq perturb 10", devname, index, index);
    /*
     * mac source MAC  0xM4M5  0xM0M1M2M3
     */
    tc_command("filter add dev ifb0 parent 1: protocol ip prio 5 u32 match u16 0x0800 ffff at -2 match u16 \
0x%2x%2x 0xffff at -4 match u32 0x%2x%2x%2x%2x 0xffffffff at -8 flowid 1:%d", mac_addr[4], mac_addr[5],
	      mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], index);
    tc_command("qdisc add dev ifb0 parent 1:%d handle %d sfq perturb 10", index, index);

    return 0;
}

int tcrule_del(char *devname, char *mac_addr)
{
    tc_command("qdisc del dev %s", devname);

    tc_command("qdisc del dev ifb0");

    system("rmmod ifb");
    system("rmmod htb");

    return 0;
}

int tc_show(char *devname)
{
    tc_command("qdisc show dev %s", devname);
    tc_command("qdisc show dev ifb0");

    tc_command("class show dev %s", devname);
    tc_command("class show dev ifb0");

    return 0;
}
