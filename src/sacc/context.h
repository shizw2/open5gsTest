/*
 * Copyright (C) 2019-2022 by Sukchan Lee <acetcom@gmail.com>
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
#include "ogs-dbi.h"
#include "ogs-sbi.h"

#include "sacc-sm.h"
#include "metrics.h"
#include "timer.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int __sacc_log_domain;

#undef OGS_LOG_DOMAIN
#define OGS_LOG_DOMAIN __sacc_log_domain

#define MAX_PEER_NUM 10
#define MAC_HEARTBEAT_LOST_CNT 5

#define SACC_PEER_STATE_OFFLINE 0
#define SACC_PEER_STATE_ONLINE  1

#define SACC_MSG_TYPE_HANDSHAKE 0 //握手
#define SACC_MSG_TYPE_HEARDBEAT 1 //心跳

#define SACC_NODE_ROLE_T0 "T0"
#define SACC_NODE_ROLE_T1 "T1"
#define SACC_NODE_ROLE_T2 "T2"

typedef struct sacc_context_s {
    const char *deviceType; //产品型号
    const char *deviceName; //设备名称
    const char *deviceSeq;  //设备序列号,同deviceId
    const char *version;
    int enable;             //可开启和关闭随遇接入功能
    int port;               //随遇接入端口
    int scanInterval;       //5秒扫描一次
    int heartbeatInterval;  //每5秒发一次心跳
    int heartbeatLost;      //丢失5次心跳，认为断线
    const char *role;          //当前设备身份标识
    int group;              //当前设备组号，身份为T0时T1时，组号为零
    int node;               //当前设备节点号，身份为T0时T1时，节点号为零
    int nodeNum;
    int inheriteEnable;     //能否有继承功能开关
    ogs_timer_t     *t_hand_shake_interval;   /* timer to send hand shake to peer node */
} sacc_context_t;


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
    int priority; //优先级，越小越优先
    int state;//是否激活
    char deviceId[64];
    char role[16];
    int heartbeatLost;
    
    //ogs_sockaddr_t *addr;    
    char *uri;
    char *heartbeat_uri;
    ogs_sbi_client_t *client;

    int num_of_ipv4;
    ogs_sockaddr_t *ipv4[OGS_SBI_MAX_NUM_OF_IP_ADDRESS];
    int num_of_ipv6;
    ogs_sockaddr_t *ipv6[OGS_SBI_MAX_NUM_OF_IP_ADDRESS];    

    ogs_supi_range_t supiRanges;
    ogs_ip_range_t staticIPRanges;

    ogs_sbi_nf_instance_t *amf_nf_instance;
    ogs_sbi_nf_instance_t *smf_nf_instance;
    ogs_sbi_nf_instance_t *udm_nf_instance;
    ogs_sbi_nf_instance_t *ausf_nf_instance;
} sacc_node_t;

void sacc_context_init(void);
void sacc_context_final(void);
sacc_context_t *sacc_self(void);

int sacc_context_parse_config(void);
int sacc_sbi_context_get_nf_info(
        OpenAPI_nf_type_e nf_type, sacc_node_t *sacc_nodes);
int sacc_context_get_nodes_info(
        const char *local, sacc_node_t *sacc_nodes);
char* sacc_msg_ToString(int msg_type);
char * sacc_node_state_ToString(int state);
int sacc_initialize_nodes(void);
void sacc_associate_peer_client(sacc_node_t *peer);
void sacc_sbi_context_init_for_udm(sacc_node_t *peer);
void sacc_sbi_context_init_for_ausf(sacc_node_t *peer);
void sacc_sbi_context_init_for_smf(sacc_node_t *peer);
void sacc_sbi_context_init_for_amf(sacc_node_t *peer);

ogs_sbi_nf_service_t *sacc_sbi_nf_service_build_default(
        ogs_sbi_nf_instance_t *nf_instance, const char *name);


#ifdef __cplusplus
}
#endif

#endif /* SACC_CONTEXT_H */
