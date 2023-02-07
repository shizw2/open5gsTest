#ifndef UDP_INI_PATH_H
#define UDP_INI_PATH_H

#include "context.h"

typedef struct module_info_s{
    uint8_t module_type;
    uint8_t module_no;
    uint8_t module_state;
}module_info_t;

//保存激活的sps
typedef struct pkt_fwd_tbl_s{
    uint8_t b_sps_num;
    module_info_t ta_sps_infos[MAX_SPS_NUM];

}pkt_fwd_tbl_t;

int sps_udp_ini_open(void);
int udp_ini_open(void);
void icps_client_recv_cb(short when, ogs_socket_t fd, void *data);
void icps_server_recv_cb(short when, ogs_socket_t fd, void *data);
void udp_ini_close(void);
int udp_ini_sendto(const void *buf, size_t len, int sps_id);

//暂时放在这里
bool add_module_info(uint8_t b_module_no);
bool delete_module_info(uint8_t b_module_no);
bool find_module_info(uint8_t b_module_no);
bool delete_module_from_array(uint8_t *pbNum , module_info_t *pt_module_info, uint8_t b_module_no);
#endif