/*
 * Copyright (C) 2023 by <Your Name>
 *
 * This file is part of <Your Project>.
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

#ifndef SACC_H
#define SACC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "proto/ogs-proto.h"
#include "app/ogs-app.h"

#include "ogs-sbi.h"

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
    int node;
    int group;
    int state;//是否激活
    char deviceId[64];
    char role[16];
    int heartbeatLost;

    ogs_sockaddr_t *addr;    
    char *uri;
    char *heartbeat_uri;
    ogs_sbi_client_t *client;
} sacc_node_t;

char* sacc_msg_ToString(int msg_type);
int sacc_initialize(const sacc_config_t *config);
void sacc_scan(void);
void sacc_heartbeat(void);

void sacc_send_request(int msg_type, sacc_node_t *peer);
void sacc_associate_peer_client(sacc_node_t *peer);
ogs_sbi_request_t *sacc_build_request(int msg_type, sacc_node_t *peer, void *data);

bool sacc_handle_request(int msg_type, ogs_sbi_stream_t *stream, ogs_sbi_message_t *recvmsg);
bool sacc_handle_response(int msg_type,ogs_sbi_message_t *recvmsg);


/*void sacc_send_handshake_request(sacc_peer_t *peer);
void sacc_send_heartbeat_request(sacc_peer_t *peer);
void sacc_associate_peer_client(sacc_peer_t *peer);
ogs_sbi_request_t *sacc_build_handshake_request(sacc_peer_t *peer, void *data);
ogs_sbi_request_t *sacc_build_heartbeat_request(sacc_peer_t *peer, void *data);
bool sacc_handle_handshake_req(ogs_sbi_stream_t *stream, ogs_sbi_message_t *recvmsg);
bool sacc_handle_handshake_resp(ogs_sbi_message_t *recvmsg);*/
#ifdef __cplusplus
}
#endif

#endif /* SACC_H */