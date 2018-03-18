#ifndef _IF_STATUS_H_
#define _IF_STATUS_H_

int get_mac(char *iface, char *mac);

int get_system_value(char *cmd, char *value);

int get_if_ip(char *iface, char *ip);

#endif
