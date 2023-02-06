#ifndef UDP_INI_PATH_H
#define UDP_INI_PATH_H

#include "context.h"

int sps_udp_ini_open(void);
int icps_udp_ini_open(void);
void icps_client_recv_cb(short when, ogs_socket_t fd, void *data);
void icps_server_recv_cb(short when, ogs_socket_t fd, void *data);
void udp_ini_close(void);
#endif