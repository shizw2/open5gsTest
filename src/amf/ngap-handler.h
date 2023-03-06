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

#ifndef AMF_NGAP_HANDLER_H
#define AMF_NGAP_HANDLER_H

#include "context.h"
#include "ngap-handler-sps.h"




#ifdef __cplusplus
extern "C" {
#endif

typedef struct NGAP_icps_send_code_s{
	struct {
	NGAP_UEContextRequest_t UEContextRequest;
	NGAP_ProcedureCode_t ProcedureCode;
	size_t size;
	uint8_t PDUsessioncount;
	uint8_t PDUsessionfailedcount;
	}h;
	uint8_t *buf;	/* Buffer with consecutive OCTET_STRING bits */
	//size_t sizebuf;
}NGAP_icps_send_code_t;


void ngap_handle_ng_setup_request(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);
void ngap_handle_initial_ue_message(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);
void ngap_handle_initial_ue_message_sps(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);

void ngap_handle_uplink_nas_transport(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);
void ngap_handle_ue_radio_capability_info_indication(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);
void ngap_handle_initial_context_setup_response(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);
void ngap_handle_initial_context_setup_failure(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);

void ngap_handle_pdu_session_resource_setup_response(
        amf_gnb_t *gnb, ogs_ngap_message_t *message,ogs_pkbuf_t *pkbuf);
void ngap_handle_pdu_session_resource_modify_response(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);
void ngap_handle_pdu_session_resource_release_response(
        amf_gnb_t *gnb, ogs_ngap_message_t *message,ogs_pkbuf_t *pkbuf);

void ngap_handle_ue_context_modification_response(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);
void ngap_handle_ue_context_modification_failure(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);

void ngap_handle_ue_context_release_request(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);
void ngap_handle_ue_context_release_complete(
        amf_gnb_t *gnb, ogs_ngap_message_t *message,ogs_pkbuf_t *pkbuf);
void ngap_handle_ue_context_release_action(ran_ue_t *ran_ue);

void ngap_handle_uplink_ran_configuration_transfer(
        amf_gnb_t *gnb, ogs_ngap_message_t *message, ogs_pkbuf_t *pkbuf);

void ngap_handle_path_switch_request(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);

void ngap_handle_handover_required(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);
void ngap_handle_handover_request_ack(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);
void ngap_handle_handover_failure(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);
void ngap_handle_handover_cancel(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);
void ngap_handle_uplink_ran_status_transfer(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);
void ngap_handle_handover_notification(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);

void ngap_handle_ran_configuration_update(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);

void ngap_handle_ng_reset(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);
void ngap_handle_error_indication(
        amf_gnb_t *gnb, ogs_ngap_message_t *message);
uint8_t spsid_find_by_tmsi(ran_ue_t           *ran_ue, uint32_t *m_tmsi);//O3
uint8_t spsid_find_by_amf_ue_ngap_id(uint64_t amf_ue_ngap_id);//O3
void ngap_icps_send_to_sps(uint8_t spsid,ran_ue_t *ran_ue, NGAP_icps_send_code_t *message);//O3
int icps_handle_rev_ini_ngap(amf_internel_msg_header_t *pmsg,ogs_pkbuf_t *pkbuf);//O3
void ngap_icps_send_to_sps_pkg(
	uint8_t sps_id,ran_ue_t *ran_ue, NGAP_icps_send_head_t *send_code_head,unsigned char * buf);






#ifdef __cplusplus
}
#endif

#endif /* AMF_NGAP_HANDLER_H */
