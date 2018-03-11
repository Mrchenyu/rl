#ifndef _TCRULE_H_
#define _TCRULE_H_

int tcrule_init(char *devname, int rate);

int tc_class_add(char *devname, int index, int rate);

int tc_filter_add_on_mac(char *devname, char *mac_addr, int index);

int tcrule_del(char *devname, char *mac_addr);

int tc_show(char *devname);

#endif
