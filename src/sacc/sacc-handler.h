/*
 * Copyright (C) 2019,2020 by Sukchan Lee <acetcom@gmail.com>
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

#ifndef SACC_NSACC_HANDLER_H
#define SACC_NSACC_HANDLER_H

#include "context.h"

#ifdef __cplusplus
extern "C" {
#endif

void sacc_scan(void);
void sacc_heartbeat(void);

void sacc_send_request(int msg_type, sacc_node_t *peer);
ogs_sbi_request_t *sacc_build_request(int msg_type, sacc_node_t *peer, void *data);

bool sacc_handle_request(int msg_type, ogs_sbi_stream_t *stream, ogs_sbi_message_t *recvmsg);
bool sacc_handle_response(int msg_type,ogs_sbi_message_t *recvmsg);

bool sacc_nnrf_nfm_send_nf_register(ogs_sbi_nf_instance_t *nf_instance);
bool sacc_nnrf_nfm_send_nf_update(ogs_sbi_nf_instance_t *nf_instance);
ogs_sbi_request_t *sacc_nnrf_nfm_build_register(ogs_sbi_nf_instance_t *nf_instance);
ogs_sbi_request_t *sacc_nnrf_nfm_build_update(ogs_sbi_nf_instance_t *nf_instance);
bool sacc_nnrf_nfm_send_nf_de_register(ogs_sbi_nf_instance_t *nf_instance);
ogs_sbi_request_t *sacc_nnrf_nfm_build_de_register(ogs_sbi_nf_instance_t *nf_instance);
void updateInheriteInfo(void);
sacc_node_t* elect_inheritor(void);

#ifdef __cplusplus
}
#endif

#endif /* SACC_NSACC_HANDLER_H */
