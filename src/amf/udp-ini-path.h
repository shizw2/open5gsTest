#ifndef UDP_INI_PATH_H
#define UDP_INI_PATH_H

#include "context.h"


#define UDP_INI_HEART_BEAT_INTERVAL         1 
#define MAX_UDP_INI_HEART_BEAT_LOST_CNT     3

typedef struct module_info_s{
    uint8_t module_type;
    uint8_t module_no;
    uint8_t module_state;
    uint8_t lost_heart_beat_cnt;
}module_info_t;

//保存激活的sps
typedef struct pkt_fwd_tbl_s{
    uint8_t b_sps_num;
    module_info_t ta_sps_infos[MAX_SPS_NUM];

}pkt_fwd_tbl_t;

int send_heart_cnt;//for sps check icps offline
int sps_udp_ini_open(void);
int udp_ini_open(void);
void icps_client_recv_cb(short when, ogs_socket_t fd, void *data);
void icps_server_recv_cb(short when, ogs_socket_t fd, void *data);
void udp_ini_close(void);

/***************icps send类函数***************/
//icps给sps发送消息时调用
int udp_ini_sendto(const void *buf, size_t len, int sps_id);
int udp_ini_msg_sendto(int msg_type, ogs_sbi_udp_header_t *header,const void *buf, size_t len, int sps_id);


/***************sps send类函数***************/
void udp_ini_send_hand_shake(void);
void udp_ini_send_supi_notify(amf_ue_t *amf_ue);
//sps给icps发送消息时调用
int udp_ini_sendto_icps(const void *buf, size_t len);
int udp_ini_msg_sendto_icps(int msg_type, ogs_sbi_udp_header_t *header,const void *buf, size_t len);


/***************sps handle类函数***************/
void udp_ini_hand_shake_check(void);
void udp_ini_handle_sbi_msg(ogs_pkbuf_t *pkbuf);

/***************icps handle类函数***************/
void udp_ini_icps_handle_hand_shake(amf_internel_msg_header_t *pmsg);
void udp_ini_icps_handle_supi_notify(ogs_pkbuf_t *pkbuf);
void udp_ini_icps_handle_sbi_msg(ogs_pkbuf_t *pkbuf);

//暂时放在这里
bool add_module_info(uint8_t b_module_no);
bool delete_module_info(uint8_t b_module_no);
bool find_module_info(uint8_t b_module_no);
module_info_t * find_module_info_ex(uint8_t b_module_no);
bool delete_module_from_array(uint8_t *pbNum , module_info_t *pt_module_info, uint8_t b_module_no);
#endif