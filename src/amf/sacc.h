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
#include "sacc_context.h"


char* sacc_msg_ToString(int msg_type);
char * sacc_node_state_ToString(int state);
int sacc_initialize(const sacc_config_t *config);
int sacc_initialize2(const sacc_config_t *config);
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

bool sacc_nnrf_nfm_send_nf_register(ogs_sbi_nf_instance_t *nf_instance);
ogs_sbi_request_t *sacc_nnrf_nfm_build_register(ogs_sbi_nf_instance_t *nf_instance);
bool sacc_nnrf_nfm_send_nf_de_register(ogs_sbi_nf_instance_t *nf_instance);
ogs_sbi_request_t *sacc_nnrf_nfm_build_de_register(ogs_sbi_nf_instance_t *nf_instance);

void sacc_sbi_register_nrfinstance_for_udm(sacc_node_t *peer);
void sacc_sbi_register_nrfinstance_for_ausf(sacc_node_t *peer);
void sacc_sbi_register_nrfinstance_for_smf(sacc_node_t *peer);
void sacc_sbi_register_nrfinstance_for_amf(sacc_node_t *peer);
ogs_sbi_nf_service_t *sacc_sbi_nf_service_build_default(
        ogs_sbi_nf_instance_t *nf_instance, const char *name);

void showsaccnodes(void);
#ifdef __cplusplus
}
#endif

#endif /* SACC_H */