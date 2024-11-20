/*
 * Copyright (C) 2019-2023 by Sukchan Lee <acetcom@gmail.com>
 *
 * This file is part of Open5GS.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SACC_CONTEXT_H
#define SACC_CONTEXT_H

#include "ogs-app.h"
#include "ogs-sbi.h"
#include "ogs-sctp.h"


#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PEER_NUM 10
#define MAC_HEARTBEAT_LOST_CNT 5

#define SACC_PEER_STATE_INIT    0
#define SACC_PEER_STATE_ONLINE  1
#define SACC_PEER_STATE_OFFLINE 2


#define SACC_MSG_TYPE_HANDSHAKE 0 //握手
#define SACC_MSG_TYPE_HEARDBEAT 1 //心跳

#define SACC_NODE_ROLE_T0 "T0"
#define SACC_NODE_ROLE_T1 "T1"
#define SACC_NODE_ROLE_T2 "T2"

typedef struct sacc_config_s {
    int enable;             //可开启和关闭随遇接入功能
    int port;               //随遇接入端口
    int scanInterval;       //5秒扫描一次
    int heartbeatInterval;  //每5秒发一次心跳
    int heartbeatLost;      //丢失5次心跳，认为断线
    char role[16];          //当前设备身份标识
    int group;              //当前设备组号，身份为T0时T1时，组号为零
    int node;               //当前设备节点号，身份为T0时T1时，节点号为零
    int nodeNum;
} sacc_config_t;

typedef struct sacc_node_s {
    int group;
    int node;    
    int state;//是否激活
    char deviceId[64];
    char role[16];
    int heartbeatLost;

    ogs_sockaddr_t *addr;    
    char *uri;
    char *heartbeat_uri;
    ogs_sbi_client_t *client;

    ogs_sbi_nf_instance_t *amf_nf_instance;
    ogs_sbi_nf_instance_t *smf_nf_instance;
    ogs_sbi_nf_instance_t *udm_nf_instance;
    ogs_sbi_nf_instance_t *ausf_nf_instance;
} sacc_node_t;

int read_csv_line(FILE *file, char *buffer, char *tokens[], int max_tokens);
void parse_json_field(char *json_str);
int get_T2_config(void);
void ogs_sbi_context_get_nf_addr(ogs_sbi_nf_instance_t *nf_instance, const char *local);
int sacc_sbi_context_get_nf_info(const char *local, const char *nrf, const char *scp,ogs_sbi_nf_instance_t *nf_instance);
int sacc_sbi_context_get_nf_info2(
        const char *local, const char * nf_name, const char *nrf, const char *scp, sacc_node_t *sacc_nodes);
#ifdef __cplusplus
}
#endif

#endif /* SACC_CONTEXT_H */
