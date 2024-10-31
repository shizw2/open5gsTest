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

typedef struct sacc_config_s {
    int enable;
    int port;
    int scanInterval;
    int heartbeatInterval;
    int heartbeatLost;
    char role[16];
    int group;
    int node;
    int nodeNum;
    ogs_sockaddr_t *addr;
} sacc_config_t;

typedef struct sacc_peer_s {
    ogs_sockaddr_t *addr;
    int state;//是否激活
    char *uri;
    char *heartbeat_uri;
    ogs_sbi_client_t *client;
} sacc_peer_t;

int sacc_initialize(const sacc_config_t *config);
void sacc_scan(void);
void sacc_heartbeat(void);
void sacc_send_handshake_request(sacc_peer_t *peer);
void sacc_send_heartbeat_request(sacc_peer_t *peer);
void sacc_associate_peer_client(sacc_peer_t *peer);
ogs_sbi_request_t *sacc_build_handshake_request(sacc_peer_t *peer, void *data);
ogs_sbi_request_t *sacc_build_heartbeat_request(sacc_peer_t *peer, void *data);

bool sacc_handle_handshake_req(ogs_sbi_stream_t *stream, ogs_sbi_message_t *recvmsg);
bool sacc_handle_handshake_resp(ogs_sbi_message_t *recvmsg);
#ifdef __cplusplus
}
#endif

#endif /* SACC_H */