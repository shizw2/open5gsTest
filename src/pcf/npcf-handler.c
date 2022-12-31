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

#include "sbi-path.h"

#include "npcf-handler.h"
#include "pcf-fd-path.h"

bool pcf_npcf_am_policy_contrtol_handle_create(pcf_ue_t *pcf_ue,
        ogs_sbi_stream_t *stream, ogs_sbi_message_t *message)
{
    OpenAPI_policy_association_request_t *PolicyAssociationRequest = NULL;
    OpenAPI_guami_t *Guami = NULL;

    uint64_t supported_features = 0;

    ogs_sbi_client_t *client = NULL;
    ogs_sockaddr_t *addr = NULL;

    ogs_assert(pcf_ue);
    ogs_assert(stream);
    ogs_assert(message);

    PolicyAssociationRequest = message->PolicyAssociationRequest;
    if (!PolicyAssociationRequest) {
        ogs_error("[%s] No PolicyAssociationRequest", pcf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                message, "[%s] No PolicyAssociationRequest", pcf_ue->supi));
        return false;
    }

    if (!PolicyAssociationRequest->notification_uri) {
        ogs_error("[%s] No notificationUri", pcf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                message, "No notificationUri", pcf_ue->supi));
        return false;
    }

    if (!PolicyAssociationRequest->supi) {
        ogs_error("[%s] No supi", pcf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                message, "No supi", pcf_ue->supi));
        return false;
    }

    if (!PolicyAssociationRequest->supp_feat) {
        ogs_error("[%s] No suppFeat", pcf_ue->supi);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                message, "No suppFeat", pcf_ue->supi));
        return false;
    }

    addr = ogs_sbi_getaddr_from_uri(PolicyAssociationRequest->notification_uri);
    if (!addr) {
        ogs_error("[%s] Invalid URI [%s]",
                pcf_ue->supi, PolicyAssociationRequest->notification_uri);
        ogs_assert(true ==
            ogs_sbi_server_send_error(stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                message, "[%s] Invalid URI", pcf_ue->supi));
        return false;
    }

    if (pcf_ue->notification_uri)
        ogs_free(pcf_ue->notification_uri);
    pcf_ue->notification_uri = ogs_strdup(
            PolicyAssociationRequest->notification_uri);
    ogs_assert(pcf_ue->notification_uri);

    client = ogs_sbi_client_find(addr);
    if (!client) {
        client = ogs_sbi_client_add(addr);
        ogs_assert(client);
    }
    OGS_SBI_SETUP_CLIENT(&pcf_ue->namf, client);

    ogs_freeaddrinfo(addr);

    supported_features =
        ogs_uint64_from_string(PolicyAssociationRequest->supp_feat);
    pcf_ue->am_policy_control_features &= supported_features;

    if (PolicyAssociationRequest->gpsi) {
        if (pcf_ue->gpsi)
            ogs_free(pcf_ue->gpsi);
        pcf_ue->gpsi = ogs_strdup(PolicyAssociationRequest->gpsi);
    }

    pcf_ue->access_type = PolicyAssociationRequest->access_type;

    if (PolicyAssociationRequest->pei) {
        char *type = NULL, *value = NULL;
        if (pcf_ue->pei)
            ogs_free(pcf_ue->pei);
        pcf_ue->pei = ogs_strdup(PolicyAssociationRequest->pei);

        type = ogs_id_get_type(pcf_ue->pei);
        ogs_assert(type);
        value = ogs_id_get_value(pcf_ue->pei);
        ogs_assert(value);

        if (strcmp(type, "imeisv") == 0) {
            ogs_assert(OGS_OK == ogs_dbi_update_imeisv(pcf_ue->supi, value));
        } else {
            ogs_fatal("Unknown Type = %s", type);
            ogs_assert_if_reached();
        }


        ogs_free(type);
        ogs_free(value);
    }

    Guami = PolicyAssociationRequest->guami;
    if (Guami && Guami->amf_id &&
        Guami->plmn_id && Guami->plmn_id->mnc && Guami->plmn_id->mcc) {
        ogs_sbi_parse_guami(&pcf_ue->guami, PolicyAssociationRequest->guami);
    }

    if (PolicyAssociationRequest->rat_type)
        pcf_ue->rat_type = PolicyAssociationRequest->rat_type;

    pcf_ue->policy_association_request =
        OpenAPI_policy_association_request_copy(
                pcf_ue->policy_association_request,
                message->PolicyAssociationRequest);

    if (PolicyAssociationRequest->ue_ambr)
        pcf_ue->subscribed_ue_ambr = OpenAPI_ambr_copy(
                pcf_ue->subscribed_ue_ambr, PolicyAssociationRequest->ue_ambr);

    ogs_assert(true ==
        pcf_ue_sbi_discover_and_send(OGS_SBI_SERVICE_TYPE_NUDR_DR, NULL,
            pcf_nudr_dr_build_query_am_data, pcf_ue, stream, NULL));

    return true;
}

bool pcf_npcf_smpolicycontrol_handle_create(pcf_sess_t *sess,
        ogs_sbi_stream_t *stream, ogs_sbi_message_t *message)
{
    int status = 0;
    char *strerror = NULL;
    pcf_ue_t *pcf_ue = NULL;

    OpenAPI_sm_policy_context_data_t *SmPolicyContextData = NULL;
    OpenAPI_snssai_t *sliceInfo = NULL;

    ogs_sbi_client_t *client = NULL;
    ogs_sockaddr_t *addr = NULL;

    ogs_assert(sess);
    pcf_ue = sess->pcf_ue;
    ogs_assert(stream);
    ogs_assert(message);

    SmPolicyContextData = message->SmPolicyContextData;
    if (!SmPolicyContextData) {
        strerror = ogs_msprintf("[%s:%d] No SmPolicyContextData",
                pcf_ue->supi, sess->psi);
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        goto cleanup;
    }

    if (!SmPolicyContextData->supi) {
        strerror = ogs_msprintf("[%s:%d] No supi", pcf_ue->supi, sess->psi);
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        goto cleanup;
    }

    if (!SmPolicyContextData->pdu_session_id) {
        strerror = ogs_msprintf("[%s:%d] No pduSessionId",
                pcf_ue->supi, sess->psi);
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        goto cleanup;
    }

    if (!SmPolicyContextData->pdu_session_type) {
        strerror = ogs_msprintf("[%s:%d] No pduSessionType",
                pcf_ue->supi, sess->psi);
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        goto cleanup;
    }

    if (!SmPolicyContextData->dnn) {
        strerror = ogs_msprintf("[%s:%d] No dnn", pcf_ue->supi, sess->psi);
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        goto cleanup;
    }

    if (!SmPolicyContextData->notification_uri) {
        strerror = ogs_msprintf("[%s:%d] No notificationUri",
                pcf_ue->supi, sess->psi);
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        goto cleanup;
    }

    if (!SmPolicyContextData->ipv4_address &&
        !SmPolicyContextData->ipv6_address_prefix) {
        strerror = ogs_msprintf(
                "[%s:%d] No IPv4 address[%p] or IPv6 prefix[%p]",
                pcf_ue->supi, sess->psi,
                SmPolicyContextData->ipv4_address,
                SmPolicyContextData->ipv6_address_prefix);
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        goto cleanup;
    }

    sliceInfo = SmPolicyContextData->slice_info;
    if (!sliceInfo) {
        strerror = ogs_msprintf("[%s:%d] No sliceInfo",
                pcf_ue->supi, sess->psi);
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        goto cleanup;
    }

    if (!sliceInfo->sst) {
        strerror = ogs_msprintf("[%s:%d] No sliceInfo->sst",
                pcf_ue->supi, sess->psi);
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        goto cleanup;
    }

    addr = ogs_sbi_getaddr_from_uri(SmPolicyContextData->notification_uri);
    if (!addr) {
        strerror = ogs_msprintf("[%s:%d] Invalid URI [%s]",
                pcf_ue->supi, sess->psi, SmPolicyContextData->notification_uri);
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        goto cleanup;
    }

    if (SmPolicyContextData->supp_feat) {
        uint64_t supported_features =
            ogs_uint64_from_string(SmPolicyContextData->supp_feat);
        sess->smpolicycontrol_features &= supported_features;
    } else {
        sess->smpolicycontrol_features = 0;
    }

    sess->pdu_session_type = SmPolicyContextData->pdu_session_type;

    if (sess->dnn)
        ogs_free(sess->dnn);
    sess->dnn = ogs_strdup(SmPolicyContextData->dnn);
    ogs_assert(sess->dnn);

    if (sess->notification_uri)
        ogs_free(sess->notification_uri);
    sess->notification_uri = ogs_strdup(SmPolicyContextData->notification_uri);
    ogs_assert(sess->notification_uri);

    client = ogs_sbi_client_find(addr);
    if (!client) {
        client = ogs_sbi_client_add(addr);
        ogs_assert(client);
    }
    OGS_SBI_SETUP_CLIENT(&sess->nsmf, client);

    ogs_freeaddrinfo(addr);

    if (SmPolicyContextData->ipv4_address)
        ogs_assert(true ==
            pcf_sess_set_ipv4addr(sess, SmPolicyContextData->ipv4_address));
    if (SmPolicyContextData->ipv6_address_prefix)
        ogs_assert(true ==
            pcf_sess_set_ipv6prefix(
                sess, SmPolicyContextData->ipv6_address_prefix));

    sess->s_nssai.sst = sliceInfo->sst;
    sess->s_nssai.sd = ogs_s_nssai_sd_from_string(sliceInfo->sd);

    if (SmPolicyContextData->subs_sess_ambr)
        sess->subscribed_sess_ambr = OpenAPI_ambr_copy(
            sess->subscribed_sess_ambr, SmPolicyContextData->subs_sess_ambr);

    if (SmPolicyContextData->subs_def_qos)
        sess->subscribed_default_qos = OpenAPI_subscribed_default_qos_copy(
            sess->subscribed_default_qos, SmPolicyContextData->subs_def_qos);

    ogs_assert(true ==
        pcf_sess_sbi_discover_and_send(
            OGS_SBI_SERVICE_TYPE_NUDR_DR, NULL,
            pcf_nudr_dr_build_query_sm_data, sess, stream, NULL));

    return true;

cleanup:
    ogs_assert(status);
    ogs_assert(strerror);
    ogs_error("%s", strerror);
    ogs_assert(true ==
        ogs_sbi_server_send_error(stream, status, message, strerror, NULL));
    ogs_free(strerror);

    return false;
}

bool pcf_npcf_smpolicycontrol_handle_delete(pcf_sess_t *sess,
        ogs_sbi_stream_t *stream, ogs_sbi_message_t *message)
{
    int rv;
    int status = 0;
    char *strerror = NULL;
    pcf_ue_t *pcf_ue = NULL;
    pcf_app_t *app_session = NULL;

    OpenAPI_sm_policy_delete_data_t *SmPolicyDeleteData = NULL;

    ogs_assert(sess);
    pcf_ue = sess->pcf_ue;
    ogs_assert(stream);
    ogs_assert(message);

    SmPolicyDeleteData = message->SmPolicyDeleteData;
    if (!SmPolicyDeleteData) {
        strerror = ogs_msprintf("[%s:%d] No SmPolicyDeleteData",
                pcf_ue->supi, sess->psi);
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        goto cleanup;
    }

    
    ogs_list_for_each(&sess->app_list, app_session) {   
        if (app_session->rx_sid != NULL){
            rv = pcf_rx_send_asr(
                    app_session->rx_sid, OGS_DIAM_RX_ABORT_CAUSE_BEARER_RELEASED);
            ogs_assert(rv == OGS_OK);
        }else{
            pcf_sbi_send_policyauthorization_terminate_notify(app_session);
        }
    }

    if (pcf_sessions_number_by_snssai_and_dnn(
                pcf_ue, &sess->s_nssai, sess->dnn) > 1) {
        ogs_sbi_message_t sendmsg;
        memset(&sendmsg, 0, sizeof(sendmsg));

        ogs_sbi_response_t *response = ogs_sbi_build_response(
                &sendmsg, OGS_SBI_HTTP_STATUS_NO_CONTENT);
        ogs_assert(response);
        ogs_assert(true == ogs_sbi_server_send_response(stream, response));
    } else {
        ogs_assert(true ==
            pcf_sess_sbi_discover_and_send(
                OGS_SBI_SERVICE_TYPE_NBSF_MANAGEMENT, NULL,
                pcf_nbsf_management_build_de_register, sess, stream, NULL));
    }

    return true;

cleanup:
    ogs_assert(status);
    ogs_assert(strerror);
    ogs_error("%s", strerror);
    ogs_assert(true ==
        ogs_sbi_server_send_error(stream, status, message, strerror, NULL));
    ogs_free(strerror);

    return false;
}

bool pcf_npcf_policyauthorization_handle_create(pcf_sess_t *sess,
        ogs_sbi_stream_t *stream, ogs_sbi_message_t *recvmsg)
{
    ogs_debug("pcf_npcf_policyauthorization_handle_create");
    int i, j, rv, status = 0;
    char *strerror = NULL;
    pcf_ue_t *pcf_ue = NULL;
    pcf_app_t *app_session = NULL;

    ogs_sbi_client_t *client = NULL;
    ogs_sockaddr_t *addr = NULL;

    OpenAPI_app_session_context_t *AppSessionContext = NULL;
    OpenAPI_app_session_context_req_data_t *AscReqData = NULL;

    uint64_t supported_features = 0;

    ogs_sbi_server_t *server = NULL;
    ogs_sbi_header_t header;
    ogs_sbi_message_t sendmsg;
    ogs_sbi_response_t *response = NULL;

    ogs_session_data_t session_data;

    ogs_ims_data_t ims_data;
    ogs_media_component_t *media_component = NULL;
    ogs_media_sub_component_t *sub = NULL;

    OpenAPI_list_t *MediaComponentList = NULL;
    OpenAPI_map_t *MediaComponentMap = NULL;
    OpenAPI_media_component_t *MediaComponent = NULL;

    OpenAPI_list_t *SubComponentList = NULL;
    OpenAPI_map_t *SubComponentMap = NULL;
    OpenAPI_media_sub_component_t *SubComponent = NULL;

    OpenAPI_list_t *fDescList = NULL;

    OpenAPI_sm_policy_decision_t SmPolicyDecision;

    OpenAPI_list_t *PccRuleList = NULL;
    OpenAPI_map_t *PccRuleMap = NULL;
    OpenAPI_pcc_rule_t *PccRule = NULL;

    OpenAPI_list_t *QosDecisionList = NULL;
    OpenAPI_map_t *QosDecisionMap = NULL;
    OpenAPI_qos_data_t *QosData = NULL;

    OpenAPI_lnode_t *node = NULL, *node2 = NULL, *node3 = NULL;

    ogs_assert(sess);
    pcf_ue = sess->pcf_ue;
    ogs_assert(stream);
    ogs_assert(recvmsg);

    server = ogs_sbi_server_from_stream(stream);
    ogs_assert(server);

    AppSessionContext = recvmsg->AppSessionContext;
    if (!AppSessionContext) {
        strerror = ogs_msprintf("[%s:%d] No AppSessionContext",
                pcf_ue->supi, sess->psi);
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        goto cleanup;
    }

    AscReqData = AppSessionContext->asc_req_data;
    if (!AscReqData) {
        strerror = ogs_msprintf("[%s:%d] No AscReqData",
                pcf_ue->supi, sess->psi);
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        goto cleanup;
    }

    if (!AscReqData->supp_feat) {
        strerror = ogs_msprintf("[%s:%d] No AscReqData->suppFeat",
                pcf_ue->supi, sess->psi);
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        goto cleanup;
    }

    if (!AscReqData->notif_uri) {
        strerror = ogs_msprintf("[%s:%d] No AscReqData->notifUri",
                pcf_ue->supi, sess->psi);
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        goto cleanup;
    }

    if (!AscReqData->med_components) {
        strerror = ogs_msprintf("[%s:%d] No AscReqData->MediaCompoenent",
                pcf_ue->supi, sess->psi);
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        goto cleanup;
    }

    addr = ogs_sbi_getaddr_from_uri(AscReqData->notif_uri);
    if (!addr) {
        strerror = ogs_msprintf("[%s:%d] Invalid URI [%s]",
                pcf_ue->supi, sess->psi, AscReqData->notif_uri);
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        goto cleanup;
    }

    supported_features = ogs_uint64_from_string(AscReqData->supp_feat);
    sess->policyauthorization_features &= supported_features;

    if (sess->policyauthorization_features != supported_features) {
        ogs_free(AscReqData->supp_feat);
        AscReqData->supp_feat =
            ogs_uint64_to_string(sess->policyauthorization_features);
        ogs_assert(AscReqData->supp_feat);
    }

    memset(&ims_data, 0, sizeof(ims_data));

    MediaComponentList = AscReqData->med_components;
    OpenAPI_list_for_each(MediaComponentList, node) {
        MediaComponentMap = node->data;
        if (MediaComponentMap) {
            MediaComponent = MediaComponentMap->value;
            if (MediaComponent) {
                media_component = &ims_data.
                    media_component[ims_data.num_of_media_component];
                media_component->media_component_number =
                    MediaComponent->med_comp_n;
                media_component->media_type = MediaComponent->med_type;
                if (MediaComponent->mar_bw_dl)
                    media_component->max_requested_bandwidth_dl =
                        ogs_sbi_bitrate_from_string(MediaComponent->mar_bw_dl);
                if (MediaComponent->mar_bw_ul)
                    media_component->max_requested_bandwidth_ul =
                        ogs_sbi_bitrate_from_string(MediaComponent->mar_bw_ul);
                if (MediaComponent->mir_bw_dl)
                    media_component->min_requested_bandwidth_dl =
                        ogs_sbi_bitrate_from_string(MediaComponent->mir_bw_dl);
                if (MediaComponent->mir_bw_ul)
                    media_component->min_requested_bandwidth_ul =
                        ogs_sbi_bitrate_from_string(MediaComponent->mir_bw_ul);
                if (MediaComponent->rr_bw)
                    media_component->rr_bandwidth =
                        ogs_sbi_bitrate_from_string(MediaComponent->rr_bw);
                if (MediaComponent->rs_bw)
                    media_component->rs_bandwidth =
                        ogs_sbi_bitrate_from_string(MediaComponent->rs_bw);
                media_component->flow_status = MediaComponent->f_status;

                SubComponentList = MediaComponent->med_sub_comps;
                OpenAPI_list_for_each(SubComponentList, node2) {
                    sub = &media_component->sub[media_component->num_of_sub];

                    SubComponentMap = node2->data;
                    if (SubComponentMap) {
                        SubComponent = SubComponentMap->value;
                        if (SubComponent) {
                            sub->flow_number = SubComponent->f_num;
                            sub->flow_usage = SubComponent->flow_usage;

                            fDescList = SubComponent->f_descs;
                            OpenAPI_list_for_each(fDescList, node3) {
                                ogs_flow_t *flow = NULL;

                                ogs_assert(sub->num_of_flow <
                                    OGS_MAX_NUM_OF_FLOW_IN_MEDIA_SUB_COMPONENT);
                                flow = &sub->flow[sub->num_of_flow];
                                if (node3->data) {
                                    flow->description = ogs_strdup(node3->data);
                                    ogs_assert(flow->description);

                                    sub->num_of_flow++;
                                }
                            }
                            media_component->num_of_sub++;
                        }
                    }
                }
                ims_data.num_of_media_component++;
            }
        }
    }

    app_session = pcf_app_add(sess);
    ogs_assert(app_session);

    if (app_session->notif_uri)
        ogs_free(app_session->notif_uri);
    app_session->notif_uri = ogs_strdup(AscReqData->notif_uri);
    ogs_assert(app_session->notif_uri);

    char buf[OGS_ADDRSTRLEN];
    //ogs_error("pcf_npcf_policyauthorization_handle_create,addr:%s,notif_uri:%s.", OGS_ADDR(addr, buf), AscReqData->notif_uri);
    client = ogs_sbi_client_find(addr);
    if (!client) {
        client = ogs_sbi_client_add(addr);
        ogs_assert(client);
    }
    OGS_SBI_SETUP_CLIENT(&app_session->naf, client);

    ogs_freeaddrinfo(addr);

    memset(&session_data, 0, sizeof(ogs_session_data_t));
    rv = ogs_dbi_session_data(
            pcf_ue->supi, &sess->s_nssai, sess->dnn, &session_data);
    if (rv != OGS_OK) {
        strerror = ogs_msprintf("[%s:%d] Cannot find SUPI in DB",
                pcf_ue->supi, sess->psi);
        status = OGS_SBI_HTTP_STATUS_NOT_FOUND;
        goto cleanup;
    }

    memset(&SmPolicyDecision, 0, sizeof(SmPolicyDecision));

    PccRuleList = OpenAPI_list_create();
    ogs_assert(PccRuleList);

    QosDecisionList = OpenAPI_list_create();
    ogs_assert(QosDecisionList);

    for (i = 0; i < ims_data.num_of_media_component; i++) {
        int flow_presence = 0;

        ogs_pcc_rule_t *pcc_rule = NULL;
        ogs_pcc_rule_t *db_pcc_rule = NULL;
        uint8_t qos_index = 0;
        ogs_media_component_t *media_component = &ims_data.media_component[i];

        switch(media_component->media_type) {
        case OpenAPI_media_type_AUDIO:
            qos_index = OGS_QOS_INDEX_1;
            break;
        case OpenAPI_media_type_VIDEO:
            qos_index = OGS_QOS_INDEX_2;
            break;
        case OpenAPI_media_type_CONTROL:
            qos_index = OGS_QOS_INDEX_5;
            break;
        default:
            strerror = ogs_msprintf("[%s:%d] Not implemented : [Media-Type:%d]",
                    pcf_ue->supi, sess->psi, media_component->media_type);
            status = OGS_SBI_HTTP_STATUS_INTERNAL_SERVER_ERROR;
            goto cleanup;
        }

        for (j = 0; j < session_data.num_of_pcc_rule; j++) {
            if (session_data.pcc_rule[j].qos.index == qos_index) {
                db_pcc_rule = &session_data.pcc_rule[j];
                break;
            }
        }

        if (!db_pcc_rule &&
            (media_component->media_type == OpenAPI_media_type_CONTROL)) {
            /*
             * Check for default bearer for IMS signalling
             * QCI 5 and ARP 1
             */
            if (session_data.session.qos.index != OGS_QOS_INDEX_5 ||
                session_data.session.qos.arp.priority_level != 1) {
                strerror = ogs_msprintf("[%s:%d] CHECK WEBUI : "
                    "Even the Default Bearer(QCI:%d,ARP:%d) "
                    "cannot support IMS signalling.",
                    pcf_ue->supi, sess->psi,
                    session_data.session.qos.index,
                    session_data.session.qos.arp.priority_level);
                status = OGS_SBI_HTTP_STATUS_FORBIDDEN;
                goto cleanup;
            } else {
                continue;
            }
        }

        if (!db_pcc_rule) {
            strerror = ogs_msprintf("[%s:%d] CHECK WEBUI : "
                "No PCC Rule in DB [QoS Index:%d] - "
                "Please add PCC Rule using WEBUI",
                pcf_ue->supi, sess->psi, qos_index);
            status = OGS_SBI_HTTP_STATUS_FORBIDDEN;
            goto cleanup;
        }

        for (j = 0; j < app_session->num_of_pcc_rule; j++) {
            if (app_session->pcc_rule[j].qos.index == qos_index) {
                pcc_rule = &app_session->pcc_rule[j];
                break;
            }
        }

        if (!pcc_rule) {
            pcc_rule = &app_session->pcc_rule[app_session->num_of_pcc_rule];
            ogs_assert(pcc_rule);

            pcc_rule->id = ogs_msprintf("%s-a%s",
                            db_pcc_rule->id, app_session->app_session_id);
            ogs_assert(pcc_rule->id);

            memcpy(&pcc_rule->qos, &db_pcc_rule->qos, sizeof(ogs_qos_t));

            pcc_rule->flow_status = db_pcc_rule->flow_status;
            pcc_rule->precedence = db_pcc_rule->precedence;

            /* Install Flow */
            flow_presence = 1;
            rv = ogs_pcc_rule_install_flow_from_media(
                    pcc_rule, media_component);
            if (rv != OGS_OK) {
                strerror = ogs_msprintf("[%s:%d] install_flow() failed",
                    pcf_ue->supi, sess->psi);
                status = OGS_SBI_HTTP_STATUS_FORBIDDEN;
                goto cleanup;
            }

            app_session->num_of_pcc_rule++;

        } else {
            int count = 0;

            /* Check Flow */
            count = ogs_pcc_rule_num_of_flow_equal_to_media(
                    pcc_rule, media_component);
            if (count == -1) {
                strerror = ogs_msprintf("[%s:%d] matched_flow() failed",
                    pcf_ue->supi, sess->psi);
                status = OGS_SBI_HTTP_STATUS_FORBIDDEN;
                goto cleanup;
            }

            if (pcc_rule->num_of_flow != count) {
                /* Re-install Flow */
                flow_presence = 1;
                rv = ogs_pcc_rule_install_flow_from_media(
                        pcc_rule, media_component);
                if (rv != OGS_OK) {
                    strerror = ogs_msprintf("[%s:%d] re-install_flow() failed",
                        pcf_ue->supi, sess->psi);
                    status = OGS_SBI_HTTP_STATUS_FORBIDDEN;
                    goto cleanup;
                }
            }
        }

        /* Update QoS */
        rv = ogs_pcc_rule_update_qos_from_media(pcc_rule, media_component);
        if (rv != OGS_OK) {
            strerror = ogs_msprintf("[%s:%d] update_qos() failed",
                pcf_ue->supi, sess->psi);
            status = OGS_SBI_HTTP_STATUS_FORBIDDEN;
            goto cleanup;
        }

        /* if we failed to get QoS from IMS, apply WEBUI QoS */
        if (pcc_rule->qos.mbr.downlink == 0)
            pcc_rule->qos.mbr.downlink = db_pcc_rule->qos.mbr.downlink;
        if (pcc_rule->qos.mbr.uplink == 0)
            pcc_rule->qos.mbr.uplink = db_pcc_rule->qos.mbr.uplink;
        if (pcc_rule->qos.gbr.downlink == 0)
            pcc_rule->qos.gbr.downlink = db_pcc_rule->qos.gbr.downlink;
        if (pcc_rule->qos.gbr.uplink == 0)
            pcc_rule->qos.gbr.uplink = db_pcc_rule->qos.gbr.uplink;

        /**************************************************************
         * Build PCC Rule & QoS Decision
         *************************************************************/
        PccRule = ogs_sbi_build_pcc_rule(pcc_rule, flow_presence);
        ogs_assert(PccRule->pcc_rule_id);

        PccRuleMap = OpenAPI_map_create(PccRule->pcc_rule_id, PccRule);
        ogs_assert(PccRuleMap);

        OpenAPI_list_add(PccRuleList, PccRuleMap);

        QosData = ogs_sbi_build_qos_data(pcc_rule);
        ogs_assert(QosData);
        ogs_assert(QosData->qos_id);

        QosDecisionMap = OpenAPI_map_create(QosData->qos_id, QosData);
        ogs_assert(QosDecisionMap);

        OpenAPI_list_add(QosDecisionList, QosDecisionMap);
    }

    if (PccRuleList->count)
        SmPolicyDecision.pcc_rules = PccRuleList;

    if (QosDecisionList->count)
        SmPolicyDecision.qos_decs = QosDecisionList;

    memset(&sendmsg, 0, sizeof(sendmsg));

    memset(&header, 0, sizeof(header));
    header.service.name = (char *)OGS_SBI_SERVICE_NAME_NPCF_POLICYAUTHORIZATION;
    header.api.version = (char *)OGS_SBI_API_V1;
    header.resource.component[0] = (char *)OGS_SBI_RESOURCE_NAME_APP_SESSIONS;
    header.resource.component[1] = (char *)app_session->app_session_id;
    sendmsg.http.location = ogs_sbi_server_uri(server, &header);
    ogs_assert(sendmsg.http.location);

    sendmsg.AppSessionContext = recvmsg->AppSessionContext;

    response = ogs_sbi_build_response(&sendmsg, OGS_SBI_HTTP_STATUS_CREATED);
    ogs_assert(response);
    ogs_assert(true == ogs_sbi_server_send_response(stream, response));

    ogs_free(sendmsg.http.location);

    if (PccRuleList->count || QosDecisionList->count) {
        ogs_assert(true == pcf_sbi_send_smpolicycontrol_update_notify(
                                sess, &SmPolicyDecision));
    }

    OpenAPI_list_for_each(PccRuleList, node) {
        PccRuleMap = node->data;
        if (PccRuleMap) {
            PccRule = PccRuleMap->value;
            if (PccRule)
                ogs_sbi_free_pcc_rule(PccRule);
            ogs_free(PccRuleMap);
        }
    }
    OpenAPI_list_free(PccRuleList);

    OpenAPI_list_for_each(QosDecisionList, node) {
        QosDecisionMap = node->data;
        if (QosDecisionMap) {
            QosData = QosDecisionMap->value;
            if (QosData)
                ogs_sbi_free_qos_data(QosData);
            ogs_free(QosDecisionMap);
        }
    }
    OpenAPI_list_free(QosDecisionList);

    ogs_ims_data_free(&ims_data);
    ogs_session_data_free(&session_data);

    return true;

cleanup:
    ogs_assert(status);
    ogs_assert(strerror);
    ogs_error("%s", strerror);
    ogs_assert(true ==
        ogs_sbi_server_send_error(stream, status, recvmsg, strerror, NULL));
    ogs_free(strerror);

    OpenAPI_list_for_each(PccRuleList, node) {
        PccRuleMap = node->data;
        if (PccRuleMap) {
            PccRule = PccRuleMap->value;
            if (PccRule)
                ogs_sbi_free_pcc_rule(PccRule);
            ogs_free(PccRuleMap);
        }
    }
    OpenAPI_list_free(PccRuleList);

    OpenAPI_list_for_each(QosDecisionList, node) {
        QosDecisionMap = node->data;
        if (QosDecisionMap) {
            QosData = QosDecisionMap->value;
            if (QosData)
                ogs_sbi_free_qos_data(QosData);
            ogs_free(QosDecisionMap);
        }
    }
    OpenAPI_list_free(QosDecisionList);

    ogs_ims_data_free(&ims_data);
    ogs_session_data_free(&session_data);

    return false;
}

bool pcf_npcf_policyauthorization_handle_update(
        pcf_sess_t *sess, pcf_app_t *app_session,
        ogs_sbi_stream_t *stream, ogs_sbi_message_t *recvmsg)
{
    int i, j, rv, status = 0;
    char *strerror = NULL;
    pcf_ue_t *pcf_ue = NULL;

    OpenAPI_app_session_context_update_data_patch_t
        *AppSessionContextUpdateDataPatch = NULL;
    OpenAPI_app_session_context_update_data_t *AscUpdateData = NULL;

    ogs_sbi_message_t sendmsg;
    ogs_sbi_response_t *response = NULL;

    ogs_session_data_t session_data;

    ogs_ims_data_t ims_data;
    ogs_media_component_t *media_component = NULL;
    ogs_media_sub_component_t *sub = NULL;

    OpenAPI_list_t *MediaComponentList = NULL;
    OpenAPI_map_t *MediaComponentMap = NULL;
    OpenAPI_media_component_rm_t *MediaComponent = NULL;

    OpenAPI_list_t *SubComponentList = NULL;
    OpenAPI_map_t *SubComponentMap = NULL;
    OpenAPI_media_sub_component_t *SubComponent = NULL;

    OpenAPI_list_t *fDescList = NULL;

    OpenAPI_sm_policy_decision_t SmPolicyDecision;

    OpenAPI_list_t *PccRuleList = NULL;
    OpenAPI_map_t *PccRuleMap = NULL;
    OpenAPI_pcc_rule_t *PccRule = NULL;

    OpenAPI_list_t *QosDecisionList = NULL;
    OpenAPI_map_t *QosDecisionMap = NULL;
    OpenAPI_qos_data_t *QosData = NULL;

    OpenAPI_lnode_t *node = NULL, *node2 = NULL, *node3 = NULL;

    ogs_assert(sess);
    pcf_ue = sess->pcf_ue;
    ogs_assert(app_session);
    ogs_assert(stream);
    ogs_assert(recvmsg);

    AppSessionContextUpdateDataPatch =
        recvmsg->AppSessionContextUpdateDataPatch;
    if (!AppSessionContextUpdateDataPatch) {
        strerror = ogs_msprintf("[%s:%d] No AppSessionContextUpdateDataPatch",
                pcf_ue->supi, sess->psi);
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        goto cleanup;
    }

    AscUpdateData = AppSessionContextUpdateDataPatch->asc_req_data;
    if (!AscUpdateData) {
        strerror = ogs_msprintf("[%s:%d] No AscUpdateData",
                pcf_ue->supi, sess->psi);
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        goto cleanup;
    }

    if (!AscUpdateData->med_components) {
        strerror = ogs_msprintf("[%s:%d] No AscUpdateData->MediaCompoenent",
                pcf_ue->supi, sess->psi);
        status = OGS_SBI_HTTP_STATUS_BAD_REQUEST;
        goto cleanup;
    }

    memset(&ims_data, 0, sizeof(ims_data));

    MediaComponentList = AscUpdateData->med_components;
    OpenAPI_list_for_each(MediaComponentList, node) {
        MediaComponentMap = node->data;
        if (MediaComponentMap) {
            MediaComponent = MediaComponentMap->value;
            if (MediaComponent) {
                media_component = &ims_data.
                    media_component[ims_data.num_of_media_component];

                media_component->media_component_number =
                    MediaComponent->med_comp_n;
                media_component->media_type = MediaComponent->med_type;
                if (MediaComponent->mar_bw_dl)
                    media_component->max_requested_bandwidth_dl =
                        ogs_sbi_bitrate_from_string(MediaComponent->mar_bw_dl);
                if (MediaComponent->mar_bw_ul)
                    media_component->max_requested_bandwidth_ul =
                        ogs_sbi_bitrate_from_string(MediaComponent->mar_bw_ul);
                if (MediaComponent->mir_bw_dl)
                    media_component->min_requested_bandwidth_dl =
                        ogs_sbi_bitrate_from_string(MediaComponent->mir_bw_dl);
                if (MediaComponent->mir_bw_ul)
                    media_component->min_requested_bandwidth_ul =
                        ogs_sbi_bitrate_from_string(MediaComponent->mir_bw_ul);
                if (MediaComponent->rr_bw)
                    media_component->rr_bandwidth =
                        ogs_sbi_bitrate_from_string(MediaComponent->rr_bw);
                if (MediaComponent->rs_bw)
                    media_component->rs_bandwidth =
                        ogs_sbi_bitrate_from_string(MediaComponent->rs_bw);
                media_component->flow_status = MediaComponent->f_status;

                SubComponentList = MediaComponent->med_sub_comps;
                OpenAPI_list_for_each(SubComponentList, node2) {
                    sub = &media_component->sub[media_component->num_of_sub];

                    SubComponentMap = node2->data;
                    if (SubComponentMap) {
                        SubComponent = SubComponentMap->value;
                        if (SubComponent) {
                            sub->flow_number = SubComponent->f_num;
                            sub->flow_usage = SubComponent->flow_usage;

                            fDescList = SubComponent->f_descs;
                            OpenAPI_list_for_each(fDescList, node3) {
                                ogs_flow_t *flow = NULL;

                                ogs_assert(sub->num_of_flow <
                                    OGS_MAX_NUM_OF_FLOW_IN_MEDIA_SUB_COMPONENT);
                                flow = &sub->flow[sub->num_of_flow];
                                if (node3->data) {
                                    flow->description = ogs_strdup(node3->data);
                                    ogs_assert(flow->description);

                                    sub->num_of_flow++;
                                }
                            }
                            media_component->num_of_sub++;
                        }
                    }
                }
                ims_data.num_of_media_component++;
            }
        }
    }

    memset(&session_data, 0, sizeof(ogs_session_data_t));
    rv = ogs_dbi_session_data(
            pcf_ue->supi, &sess->s_nssai, sess->dnn, &session_data);
    if (rv != OGS_OK) {
        strerror = ogs_msprintf("[%s:%d] Cannot find SUPI in DB",
                pcf_ue->supi, sess->psi);
        status = OGS_SBI_HTTP_STATUS_NOT_FOUND;
        goto cleanup;
    }

    memset(&SmPolicyDecision, 0, sizeof(SmPolicyDecision));

    PccRuleList = OpenAPI_list_create();
    ogs_assert(PccRuleList);

    QosDecisionList = OpenAPI_list_create();
    ogs_assert(QosDecisionList);

    for (i = 0; i < ims_data.num_of_media_component; i++) {
        int flow_presence = 0;

        ogs_pcc_rule_t *pcc_rule = NULL;
        ogs_pcc_rule_t *db_pcc_rule = NULL;
        uint8_t qos_index = 0;
        ogs_media_component_t *media_component = &ims_data.media_component[i];

        switch(media_component->media_type) {
        case OpenAPI_media_type_AUDIO:
            qos_index = OGS_QOS_INDEX_1;
            break;
        case OpenAPI_media_type_VIDEO:
            qos_index = OGS_QOS_INDEX_2;
            break;
        case OpenAPI_media_type_CONTROL:
            qos_index = OGS_QOS_INDEX_5;
            break;
        default:
            strerror = ogs_msprintf("[%s:%d] Not implemented : [Media-Type:%d]",
                    pcf_ue->supi, sess->psi, media_component->media_type);
            status = OGS_SBI_HTTP_STATUS_INTERNAL_SERVER_ERROR;
            goto cleanup;
        }

        for (j = 0; j < session_data.num_of_pcc_rule; j++) {
            if (session_data.pcc_rule[j].qos.index == qos_index) {
                db_pcc_rule = &session_data.pcc_rule[j];
                break;
            }
        }

        if (!db_pcc_rule &&
            (media_component->media_type == OpenAPI_media_type_CONTROL)) {
            /*
             * Check for default bearer for IMS signalling
             * QCI 5 and ARP 1
             */
            if (session_data.session.qos.index != OGS_QOS_INDEX_5 ||
                session_data.session.qos.arp.priority_level != 1) {
                strerror = ogs_msprintf("[%s:%d] CHECK WEBUI : "
                    "Even the Default Bearer(QCI:%d,ARP:%d) "
                    "cannot support IMS signalling.",
                    pcf_ue->supi, sess->psi,
                    session_data.session.qos.index,
                    session_data.session.qos.arp.priority_level);
                status = OGS_SBI_HTTP_STATUS_FORBIDDEN;
                goto cleanup;
            } else {
                continue;
            }
        }

        if (!db_pcc_rule) {
            strerror = ogs_msprintf("[%s:%d] CHECK WEBUI : "
                "No PCC Rule in DB [QoS Index:%d] - "
                "Please add PCC Rule using WEBUI",
                pcf_ue->supi, sess->psi, qos_index);
            status = OGS_SBI_HTTP_STATUS_FORBIDDEN;
            goto cleanup;
        }

        for (j = 0; j < app_session->num_of_pcc_rule; j++) {
            if (app_session->pcc_rule[j].qos.index == qos_index) {
                pcc_rule = &app_session->pcc_rule[j];
                break;
            }
        }

        if (!pcc_rule) {
            pcc_rule = &app_session->pcc_rule[app_session->num_of_pcc_rule];
            ogs_assert(pcc_rule);

            pcc_rule->id = ogs_strdup(app_session->app_session_id);
            ogs_assert(pcc_rule->id);

            memcpy(&pcc_rule->qos, &db_pcc_rule->qos, sizeof(ogs_qos_t));

            pcc_rule->flow_status = db_pcc_rule->flow_status;
            pcc_rule->precedence = db_pcc_rule->precedence;

            /* Install Flow */
            flow_presence = 1;
            rv = ogs_pcc_rule_install_flow_from_media(
                    pcc_rule, media_component);
            if (rv != OGS_OK) {
                strerror = ogs_msprintf("[%s:%d] install_flow() failed",
                    pcf_ue->supi, sess->psi);
                status = OGS_SBI_HTTP_STATUS_FORBIDDEN;
                goto cleanup;
            }

            app_session->num_of_pcc_rule++;

        } else {
            int count = 0;

            /* Check Flow */
            count = ogs_pcc_rule_num_of_flow_equal_to_media(
                    pcc_rule, media_component);
            if (count == -1) {
                strerror = ogs_msprintf("[%s:%d] matched_flow() failed",
                    pcf_ue->supi, sess->psi);
                status = OGS_SBI_HTTP_STATUS_FORBIDDEN;
                goto cleanup;
            }

            if (pcc_rule->num_of_flow != count) {
                /* Re-install Flow */
                flow_presence = 1;
                rv = ogs_pcc_rule_install_flow_from_media(
                        pcc_rule, media_component);
                if (rv != OGS_OK) {
                    strerror = ogs_msprintf("[%s:%d] re-install_flow() failed",
                        pcf_ue->supi, sess->psi);
                    status = OGS_SBI_HTTP_STATUS_FORBIDDEN;
                    goto cleanup;
                }
            }
        }

        /* Update QoS */
        rv = ogs_pcc_rule_update_qos_from_media(pcc_rule, media_component);
        if (rv != OGS_OK) {
            strerror = ogs_msprintf("[%s:%d] update_qos() failed",
                pcf_ue->supi, sess->psi);
            status = OGS_SBI_HTTP_STATUS_FORBIDDEN;
            goto cleanup;
        }

        /* if we failed to get QoS from IMS, apply WEBUI QoS */
        if (pcc_rule->qos.mbr.downlink == 0)
            pcc_rule->qos.mbr.downlink = db_pcc_rule->qos.mbr.downlink;
        if (pcc_rule->qos.mbr.uplink == 0)
            pcc_rule->qos.mbr.uplink = db_pcc_rule->qos.mbr.uplink;
        if (pcc_rule->qos.gbr.downlink == 0)
            pcc_rule->qos.gbr.downlink = db_pcc_rule->qos.gbr.downlink;
        if (pcc_rule->qos.gbr.uplink == 0)
            pcc_rule->qos.gbr.uplink = db_pcc_rule->qos.gbr.uplink;

        /**************************************************************
         * Build PCC Rule & QoS Decision
         *************************************************************/
        PccRule = ogs_sbi_build_pcc_rule(pcc_rule, flow_presence);
        ogs_assert(PccRule->pcc_rule_id);

        PccRuleMap = OpenAPI_map_create(PccRule->pcc_rule_id, PccRule);
        ogs_assert(PccRuleMap);

        OpenAPI_list_add(PccRuleList, PccRuleMap);

        QosData = ogs_sbi_build_qos_data(pcc_rule);
        ogs_assert(QosData);
        ogs_assert(QosData->qos_id);

        QosDecisionMap = OpenAPI_map_create(QosData->qos_id, QosData);
        ogs_assert(QosDecisionMap);

        OpenAPI_list_add(QosDecisionList, QosDecisionMap);
    }

    if (PccRuleList->count)
        SmPolicyDecision.pcc_rules = PccRuleList;

    if (QosDecisionList->count)
        SmPolicyDecision.qos_decs = QosDecisionList;

    memset(&sendmsg, 0, sizeof(sendmsg));

    sendmsg.AppSessionContextUpdateDataPatch =
        recvmsg->AppSessionContextUpdateDataPatch;

    response = ogs_sbi_build_response(&sendmsg, OGS_SBI_HTTP_STATUS_OK);
    ogs_assert(response);
    ogs_assert(true == ogs_sbi_server_send_response(stream, response));

    if (PccRuleList->count || QosDecisionList->count) {
        ogs_assert(true == pcf_sbi_send_smpolicycontrol_update_notify(
                            sess, &SmPolicyDecision));
    }

    OpenAPI_list_for_each(PccRuleList, node) {
        PccRuleMap = node->data;
        if (PccRuleMap) {
            PccRule = PccRuleMap->value;
            if (PccRule)
                ogs_sbi_free_pcc_rule(PccRule);
            ogs_free(PccRuleMap);
        }
    }
    OpenAPI_list_free(PccRuleList);

    OpenAPI_list_for_each(QosDecisionList, node) {
        QosDecisionMap = node->data;
        if (QosDecisionMap) {
            QosData = QosDecisionMap->value;
            if (QosData)
                ogs_sbi_free_qos_data(QosData);
            ogs_free(QosDecisionMap);
        }
    }
    OpenAPI_list_free(QosDecisionList);

    ogs_ims_data_free(&ims_data);
    ogs_session_data_free(&session_data);

    return true;

cleanup:
    ogs_assert(status);
    ogs_assert(strerror);
    ogs_error("%s", strerror);
    ogs_assert(true ==
        ogs_sbi_server_send_error(stream, status, recvmsg, strerror, NULL));
    ogs_free(strerror);

    OpenAPI_list_for_each(PccRuleList, node) {
        PccRuleMap = node->data;
        if (PccRuleMap) {
            PccRule = PccRuleMap->value;
            if (PccRule)
                ogs_sbi_free_pcc_rule(PccRule);
            ogs_free(PccRuleMap);
        }
    }
    OpenAPI_list_free(PccRuleList);

    OpenAPI_list_for_each(QosDecisionList, node) {
        QosDecisionMap = node->data;
        if (QosDecisionMap) {
            QosData = QosDecisionMap->value;
            if (QosData)
                ogs_sbi_free_qos_data(QosData);
            ogs_free(QosDecisionMap);
        }
    }
    OpenAPI_list_free(QosDecisionList);

    ogs_ims_data_free(&ims_data);
    ogs_session_data_free(&session_data);

    return false;
}

bool pcf_npcf_policyauthorization_handle_delete(
        pcf_sess_t *sess, pcf_app_t *app_session,
        ogs_sbi_stream_t *stream, ogs_sbi_message_t *recvmsg)
{
    int i;

    OpenAPI_sm_policy_decision_t SmPolicyDecision;

    OpenAPI_list_t *PccRuleList = NULL;
    OpenAPI_map_t *PccRuleMap = NULL;

    OpenAPI_list_t *QosDecisionList = NULL;
    OpenAPI_map_t *QosDecisionMap = NULL;

    OpenAPI_lnode_t *node = NULL;

    ogs_assert(true == ogs_sbi_send_http_status_no_content(stream));

    ogs_assert(app_session);

    memset(&SmPolicyDecision, 0, sizeof(SmPolicyDecision));

    PccRuleList = OpenAPI_list_create();
    ogs_assert(PccRuleList);

    QosDecisionList = OpenAPI_list_create();
    ogs_assert(QosDecisionList);

    for (i = 0; i < app_session->num_of_pcc_rule; i++) {
        ogs_pcc_rule_t *pcc_rule = &app_session->pcc_rule[i];

        ogs_assert(pcc_rule);

        PccRuleMap = OpenAPI_map_create(pcc_rule->id, NULL);
        ogs_assert(PccRuleMap);

        OpenAPI_list_add(PccRuleList, PccRuleMap);

        QosDecisionMap = OpenAPI_map_create(pcc_rule->id, NULL);
        ogs_assert(QosDecisionMap);

        OpenAPI_list_add(QosDecisionList, QosDecisionMap);
    }

    if (PccRuleList->count)
        SmPolicyDecision.pcc_rules = PccRuleList;

    if (QosDecisionList->count)
        SmPolicyDecision.qos_decs = QosDecisionList;

    if (PccRuleList->count || QosDecisionList->count) {
        ogs_assert(true == pcf_sbi_send_smpolicycontrol_delete_notify(
                            sess, app_session, &SmPolicyDecision));
    } else {
        pcf_app_remove(app_session);
    }

    OpenAPI_list_for_each(PccRuleList, node) {
        PccRuleMap = node->data;
        if (PccRuleMap) {
            ogs_free(PccRuleMap);
        }
    }
    OpenAPI_list_free(PccRuleList);

    OpenAPI_list_for_each(QosDecisionList, node) {
        QosDecisionMap = node->data;
        if (QosDecisionMap) {
            ogs_free(QosDecisionMap);
        }
    }
    OpenAPI_list_free(QosDecisionList);

    return true;
}

struct rx_sess_state {
    ogs_lnode_t         node;

    os0_t               sid;                            /* Rx Session-Id */

    ogs_pcc_rule_t      pcc_rule[OGS_MAX_NUM_OF_PCC_RULE];
    int                 num_of_pcc_rule;

    struct sess_state   *gx;
};

//跟pcf-rx-path.c中的sess_state不一样
struct sess_state {
    os0_t       sid;                /* Gx Session-Id */

    os0_t       peer_host;          /* Peer Host */

    char        *imsi_bcd;
    char        *apn;

ED3(uint8_t     ipv4:1;,
    uint8_t     ipv6:1;,
    uint8_t     reserved:6;)
    uint32_t    addr;                   /* Framed-IPv4-Address */
    uint8_t     addr6[OGS_IPV6_LEN];    /* Framed-IPv6-Prefix */

    ogs_list_t  rx_list;

    struct timespec ts;             /* Time of sending the message */
};

#if 1
int pcf_n7_send_rar_to_main_thread(pcf_sess_t *sess,
        pcf_app_t *app_session, ogs_diam_rx_message_t *rx_message)
{
    int rv;

    pcf_event_t *e = NULL;

    e = pcf_event_new(PCF_EVENT_RX_CMD);
    ogs_assert(e);
    e->sess = sess;         
    e->app = app_session;
    e->rx_message = rx_message;

    rv = ogs_queue_push(ogs_app()->queue, e);
    if (rv != OGS_OK) {
        ogs_error("ogs_queue_push() failed:%d", (int)rv);        
        ogs_event_free(e);
    } else {
        ogs_pollset_notify(ogs_app()->pollset);
    }

    return OGS_OK;
}
#endif

//模仿pcrf_gx_send_rar
int pcf_n7_send_rar(pcf_sess_t *sess,pcf_app_t *app_session, ogs_diam_rx_message_t *rx_message)
{
    OpenAPI_list_t *PccRuleList = NULL;
    OpenAPI_map_t *PccRuleMap = NULL;
    OpenAPI_pcc_rule_t *PccRule = NULL;

    OpenAPI_list_t *QosDecisionList = NULL;
    OpenAPI_map_t *QosDecisionMap = NULL;
    OpenAPI_qos_data_t *QosData = NULL;
    OpenAPI_lnode_t *node = NULL;//, *node2 = NULL, *node3 = NULL;
    OpenAPI_sm_policy_decision_t SmPolicyDecision;

    int rv;
    int i, j;
    pcf_ue_t *pcf_ue = NULL;

    ogs_session_data_t session_data;

    ogs_assert(rx_message);

    pcf_ue = sess->pcf_ue;

    ogs_info("[PCF] Re-Auth-Request");

    memset(&SmPolicyDecision, 0, sizeof(SmPolicyDecision));
    PccRuleList = OpenAPI_list_create();
    ogs_assert(PccRuleList);

    QosDecisionList = OpenAPI_list_create();
    ogs_assert(QosDecisionList);

    /* Set default error result code */
    rx_message->result_code = OGS_DIAM_UNKNOWN_SESSION_ID;

#if 0
    /* Initialize Message */
    memset(&gx_message, 0, sizeof(ogs_diam_gx_message_t));

    /* Create the request */
    ret = fd_msg_new(ogs_diam_gx_cmd_rar, MSGFL_ALLOC_ETEID, &req);
    ogs_assert(ret == 0);
    {
        struct msg_hdr * h;
        ret = fd_msg_hdr( req, &h );
        ogs_assert(ret == 0);
        h->msg_appl = OGS_DIAM_GX_APPLICATION_ID;
    }

    /* Retrieve session by Session-Id */
    sidlen = strlen((char *)gx_sid);
    ret = fd_sess_fromsid_msg((os0_t)gx_sid, sidlen, &session, &new);
    ogs_assert(ret == 0);
    if (new) {
        ogs_error("No session data");
        ret = fd_msg_free(req);
        ogs_assert(ret == 0);
        rx_message->result_code = OGS_DIAM_UNKNOWN_PEER;
        return OGS_ERROR;
    }

    /* Add Session-Id to the message */
    ret = ogs_diam_message_session_id_set(req, (os0_t)gx_sid, sidlen);
    ogs_assert(ret == 0);

    /* Save the session associated with the message */
    ret = fd_msg_sess_set(req, session);
    ogs_assert(ret == 0);

    /* Retrieve session state in this session */
    ret = fd_sess_state_retrieve(pcrf_gx_reg, session, &sess_data);
    ogs_assert(ret == 0);
    if (sess_data == NULL) {
        ogs_error("No session data");
        ret = fd_msg_free(req);
        ogs_assert(ret == 0);
        rx_message->result_code = OGS_DIAM_UNKNOWN_SESSION_ID;
        return OGS_ERROR;
    }
#endif


    /* Find RX session state */    
    if (rx_message->cmd_code == OGS_DIAM_RX_CMD_CODE_AA) {
    #if 0
        if (!rx_sess_data) {
            rx_sess_data = add_rx_state(sess_data, rx_sid);
            ogs_assert(rx_sess_data);
        }
    #endif
        ogs_info("OGS_DIAM_RX_CMD_CODE_AA");

        if (!app_session){
            /*add begin*/
            //将rx_sid存入到pcf_sess的app_list中
            app_session = pcf_app_add(sess);
            ogs_assert(app_session);
            

            printf("app_session id:%s\r\n",app_session->app_session_id);
            fflush(stdout);
        }
        /*add end*/

#if 0
        /* Retrieve QoS Data from Database */
        rv = pcf_db_qos_data(
                sess_data->imsi_bcd, sess_data->apn, &gx_message.session_data);
        if (rv != OGS_OK) {
            ogs_error("Cannot get data for IMSI(%s)+APN(%s)'",
                    sess_data->imsi_bcd, sess_data->apn);
            rx_message->result_code =
                OGS_DIAM_RX_DIAMETER_IP_CAN_SESSION_NOT_AVAILABLE;
            goto out;
        }
#endif

/*add begin*/
        memset(&session_data, 0, sizeof(ogs_session_data_t));
        rv = ogs_dbi_session_data(
                pcf_ue->supi, &sess->s_nssai, sess->dnn, &session_data);
        if (rv != OGS_OK) {
            ogs_error("[%s:%d] Cannot find SUPI in DB",
                    pcf_ue->supi, sess->psi);
            rx_message->result_code =
                OGS_DIAM_RX_DIAMETER_IP_CAN_SESSION_NOT_AVAILABLE;
            goto out;
        }
/*add end*/

        /* Match Media-Component with PCC Rule */
        for (i = 0; i < rx_message->ims_data.num_of_media_component; i++) {
            int flow_presence = 0;
            ogs_pcc_rule_t *pcc_rule = NULL;
            ogs_pcc_rule_t *db_pcc_rule = NULL;
            uint8_t qos_index = 0;
            ogs_media_component_t *media_component =
                &rx_message->ims_data.media_component[i];

            switch(media_component->media_type) {
            case OGS_DIAM_RX_MEDIA_TYPE_AUDIO:
                qos_index = OGS_QOS_INDEX_1;
                break;
            case OGS_DIAM_RX_MEDIA_TYPE_VIDEO:
                qos_index = OGS_QOS_INDEX_2;
                break;
            case OGS_DIAM_RX_MEDIA_TYPE_CONTROL:
                qos_index = OGS_QOS_INDEX_5;
                break;
            default:
                ogs_error("Not implemented : [Media-Type:%d]",
                        media_component->media_type);
                rx_message->result_code = OGS_DIAM_INVALID_AVP_VALUE;
                goto out;
            }
            
            for (j = 0; j < session_data.num_of_pcc_rule; j++) {
                if (session_data.pcc_rule[j].qos.index ==
                        qos_index) {
                    db_pcc_rule = &session_data.pcc_rule[j];
                    break;
                }
            }

            if (!db_pcc_rule && (media_component->media_type
                    == OGS_DIAM_RX_MEDIA_TYPE_CONTROL)) {
                /*
                 * Check for default bearer for IMS signalling
                 * QCI 5 and ARP 1
                 */
                if (session_data.
                        session.qos.index != OGS_QOS_INDEX_5 ||
                    session_data.
                        session.qos.arp.priority_level != 1) {
                    ogs_error("CHECK WEBUI : Even the Default "
                        "Bearer(QCI:%d,ARP:%d) cannot support IMS signalling.",
                        session_data.session.qos.index,
                        session_data.session.qos.arp.priority_level);
                    rx_message->result_code =
                        OGS_DIAM_RX_DIAMETER_REQUESTED_SERVICE_NOT_AUTHORIZED;
                    goto out;
                } else {
                    continue;
                }
            }

            if (!db_pcc_rule) {
                ogs_error("CHECK WEBUI : No PCC Rule in DB [QoS Index:%d]",
                        qos_index);
                ogs_error("Please add PCC Rule using WEBUI");
                rx_message->result_code = 
                    OGS_DIAM_RX_DIAMETER_REQUESTED_SERVICE_NOT_AUTHORIZED;
                goto out;
            }

            ogs_debug("db_pcc_rule->id:%s, %p.",db_pcc_rule->id,db_pcc_rule->id);

            for (j = 0; j < app_session->num_of_pcc_rule; j++) {
                if (app_session->pcc_rule[j].qos.index == qos_index) {
                    pcc_rule = &app_session->pcc_rule[j];
                    break;
                }
            }

            if (!pcc_rule) {
                pcc_rule = &app_session->pcc_rule[app_session->num_of_pcc_rule];
                ogs_assert(pcc_rule);

                #if 0
                /* Device PCC Rule Info from DB Profile */
                pcc_rule->name = ogs_msprintf("%s-r%d", db_pcc_rule->name,
                    (int)ogs_pool_index(&rx_sess_state_pool, rx_sess_data));
                ogs_assert(pcc_rule->name);
                #endif
                pcc_rule->id = ogs_msprintf("%s-a%s",
                db_pcc_rule->id, app_session->app_session_id);
                ogs_assert(pcc_rule->id);

                memcpy(&pcc_rule->qos, &db_pcc_rule->qos, sizeof(ogs_qos_t));

                pcc_rule->flow_status = db_pcc_rule->flow_status;
                pcc_rule->precedence = db_pcc_rule->precedence;

                /* Install Flow */
                flow_presence = 1;
                rv = ogs_pcc_rule_install_flow_from_media(
                        pcc_rule, media_component);
                if (rv != OGS_OK) {
                    rx_message->result_code =
                        OGS_DIAM_RX_DIAMETER_FILTER_RESTRICTIONS;
                    ogs_error("install_flow() failed");
                    goto out;
                }

                app_session->num_of_pcc_rule++;
            } else {
                int count = 0;

                /* Check Flow */
                count = ogs_pcc_rule_num_of_flow_equal_to_media(
                        pcc_rule, media_component);
                if (count == -1) {
                    rx_message->result_code =
                        OGS_DIAM_RX_DIAMETER_FILTER_RESTRICTIONS;
                    ogs_error("matched_flow() failed");
                    goto out;
                }

                if (pcc_rule->num_of_flow != count) {
                    /* Re-install Flow */
                    flow_presence = 1;
                    rv = ogs_pcc_rule_install_flow_from_media(
                            pcc_rule, media_component);
                    if (rv != OGS_OK) {
                        rx_message->result_code = 
                            OGS_DIAM_RX_DIAMETER_FILTER_RESTRICTIONS;
                        ogs_error("install_flow() failed");
                        goto out;
                    }
                }
            }

            /* Update QoS */
            rv = ogs_pcc_rule_update_qos_from_media(pcc_rule, media_component);
            if (rv != OGS_OK) {
                rx_message->result_code =
                    OGS_DIAM_RX_DIAMETER_REQUESTED_SERVICE_NOT_AUTHORIZED;
                ogs_error("update_qos() failed");
                goto out;
            }

            /* if we failed to get QoS from IMS, apply WEBUI QoS */
            if (pcc_rule->qos.mbr.downlink == 0)
                pcc_rule->qos.mbr.downlink = db_pcc_rule->qos.mbr.downlink;
            if (pcc_rule->qos.mbr.uplink == 0)
                pcc_rule->qos.mbr.uplink = db_pcc_rule->qos.mbr.uplink;
            if (pcc_rule->qos.gbr.downlink == 0)
                pcc_rule->qos.gbr.downlink = db_pcc_rule->qos.gbr.downlink;
            if (pcc_rule->qos.gbr.uplink == 0)
                pcc_rule->qos.gbr.uplink = db_pcc_rule->qos.gbr.uplink;
#if 0
            if (charging_rule == 0) {
                ret = fd_msg_avp_new(
                        ogs_diam_gx_charging_rule_install, 0, &avp);
                ogs_assert(ret == 0);
                charging_rule = 1;
            }

            rv = encode_pcc_rule_definition(avp, pcc_rule, flow_presence);
            ogs_assert(rv == OGS_OK);
#endif
            //printf("pcc_rule->id %s,pcc_rule->name %s,pcc_rule->num_of_flow %d,pcc_rule->type%d,\r\n",pcc_rule->id,pcc_rule->name,pcc_rule->num_of_flow,pcc_rule->type);
            //fflush(stdout);
            /**************************************************************
             * Build PCC Rule & QoS Decision
             *************************************************************/
            PccRule = ogs_sbi_build_pcc_rule(pcc_rule, flow_presence);
            ogs_assert(PccRule->pcc_rule_id);

            PccRuleMap = OpenAPI_map_create(PccRule->pcc_rule_id, PccRule);
            ogs_assert(PccRuleMap);

            OpenAPI_list_add(PccRuleList, PccRuleMap);

            QosData = ogs_sbi_build_qos_data(pcc_rule);
            ogs_assert(QosData);
            ogs_assert(QosData->qos_id);

            QosDecisionMap = OpenAPI_map_create(QosData->qos_id, QosData);
            ogs_assert(QosDecisionMap);

            OpenAPI_list_add(QosDecisionList, QosDecisionMap);
        }
#if 0
        if (charging_rule == 1) {
            ret = fd_msg_avp_add(req, MSG_BRW_LAST_CHILD, avp);
            ogs_assert(ret == 0);
        }
#endif
    } else if (rx_message->cmd_code ==
            OGS_DIAM_RX_CMD_CODE_SESSION_TERMINATION) {
        ogs_assert(app_session);

        for (i = 0; i < app_session->num_of_pcc_rule; i++) {
        #if 0
            ogs_assert(app_session->pcc_rule[i].name); 

            if (charging_rule == 0) {
                ret = fd_msg_avp_new(ogs_diam_gx_charging_rule_remove, 0, &avp);
                ogs_assert(ret == 0);
                charging_rule = 1;
            }

            ret = fd_msg_avp_new(ogs_diam_gx_charging_rule_name, 0, &avpch1);
            ogs_assert(ret == 0);
            val.os.data = (uint8_t *)app_session->pcc_rule[i].id;
            val.os.len = strlen(app_session->pcc_rule[i].id);
            ret = fd_msg_avp_setvalue(avpch1, &val);
            ogs_assert(ret == 0);
            ret = fd_msg_avp_add(avp, MSG_BRW_LAST_CHILD, avpch1);
            ogs_assert(ret == 0);
        #endif    
            ogs_pcc_rule_t *pcc_rule = &app_session->pcc_rule[i];

            ogs_assert(pcc_rule);

            PccRuleMap = OpenAPI_map_create(pcc_rule->id, NULL);
            ogs_assert(PccRuleMap);

            OpenAPI_list_add(PccRuleList, PccRuleMap);

            QosDecisionMap = OpenAPI_map_create(pcc_rule->id, NULL);
            ogs_assert(QosDecisionMap);

            OpenAPI_list_add(QosDecisionList, QosDecisionMap);
        }
#if 0
        if (charging_rule == 1) {
            ret = fd_msg_avp_add(req, MSG_BRW_LAST_CHILD, avp);
            ogs_assert(ret == 0);
        }

        remove_rx_state(rx_sess_data);
#endif        
    } else
        ogs_assert_if_reached();

#if 0
    ogs_debug("charging_rule:%d",charging_rule);
    if (charging_rule == 0) {
        rx_message->result_code = ER_DIAMETER_SUCCESS;
        goto out;
    }
#endif

    if (PccRuleList->count)
        SmPolicyDecision.pcc_rules = PccRuleList;

    if (QosDecisionList->count)
        SmPolicyDecision.qos_decs = QosDecisionList;

    ogs_info("pcf_sbi_send_smpolicycontrol_update_notify begin,%lu,%lu.",PccRuleList->count,QosDecisionList->count);
    if (PccRuleList->count || QosDecisionList->count) {
        if (rx_message->cmd_code == OGS_DIAM_RX_CMD_CODE_AA) {
            ogs_assert(true == pcf_sbi_send_smpolicycontrol_update_notify(
                                    sess, &SmPolicyDecision)); 

            ogs_session_data_free(&session_data);
            
        }else if(rx_message->cmd_code ==
            OGS_DIAM_RX_CMD_CODE_SESSION_TERMINATION) {
            ogs_assert(true == pcf_sbi_send_smpolicycontrol_delete_notify(
                    sess, app_session, &SmPolicyDecision));            
        }
    }

    ogs_info("pcf_sbi_send_smpolicycontrol_update_notify end.");

    OpenAPI_list_for_each(PccRuleList, node) {
        PccRuleMap = node->data;
        if (PccRuleMap) {
            PccRule = PccRuleMap->value;
            if (PccRule)
                ogs_sbi_free_pcc_rule(PccRule);
            ogs_free(PccRuleMap);
        }
    }
    OpenAPI_list_free(PccRuleList);

    OpenAPI_list_for_each(QosDecisionList, node) {
        QosDecisionMap = node->data;
        if (QosDecisionMap) {
            QosData = QosDecisionMap->value;
            if (QosData)
                ogs_sbi_free_qos_data(QosData);
            ogs_free(QosDecisionMap);
        }
    }
    OpenAPI_list_free(QosDecisionList);
   

    return OGS_OK;

#if 0
    /* Set Origin-Host & Origin-Realm */
    ret = fd_msg_add_origin(req, 0);
    ogs_assert(ret == 0);

    /* Set the Destination-Host AVP */
    ret = fd_msg_avp_new(ogs_diam_destination_host, 0, &avp);
    ogs_assert(ret == 0);
    val.os.data = sess_data->peer_host;
    val.os.len  = strlen((char *)sess_data->peer_host);
    ret = fd_msg_avp_setvalue(avp, &val);
    ogs_assert(ret == 0);
    ret = fd_msg_avp_add(req, MSG_BRW_LAST_CHILD, avp);
    ogs_assert(ret == 0);

    /* Set the Destination-Realm AVP */
    ret = fd_msg_avp_new(ogs_diam_destination_realm, 0, &avp);
    ogs_assert(ret == 0);
    val.os.data = (unsigned char *)(fd_g_config->cnf_diamrlm);
    val.os.len  = strlen(fd_g_config->cnf_diamrlm);
    ret = fd_msg_avp_setvalue(avp, &val);
    ogs_assert(ret == 0);
    ret = fd_msg_avp_add(req, MSG_BRW_LAST_CHILD, avp);
    ogs_assert(ret == 0);

    /* Set the Auth-Application-Id AVP */
    ret = fd_msg_avp_new(ogs_diam_auth_application_id, 0, &avp);
    ogs_assert(ret == 0);
    val.i32 = OGS_DIAM_GX_APPLICATION_ID;
    ret = fd_msg_avp_setvalue(avp, &val);
    ogs_assert(ret == 0);
    ret = fd_msg_avp_add(req, MSG_BRW_LAST_CHILD, avp);
    ogs_assert(ret == 0);

    /* Set the Re-Auth-Request-Type */
    ret = fd_msg_avp_new(ogs_diam_re_auth_request_type, 0, &avp);
    ogs_assert(ret == 0);
    val.i32 = OGS_DIAM_RE_AUTH_REQUEST_TYPE_AUTHORIZE_ONLY;
    ret = fd_msg_avp_setvalue(avp, &val);
    ogs_assert(ret == 0);
    ret = fd_msg_avp_add(req, MSG_BRW_LAST_CHILD, avp);
    ogs_assert(ret == 0);

    ret = clock_gettime(CLOCK_REALTIME, &sess_data->ts);
    ogs_assert(ret == 0);
    
    /* Keep a pointer to the session data for debug purpose, 
     * in real life we would not need it */
    svg = sess_data;
    
    /* Store this value in the session */
    ret = fd_sess_state_store(pcrf_gx_reg, session, &sess_data);
    ogs_assert(ret == 0);
    ogs_assert(sess_data == NULL);
    
    /* Send the request */
    ret = fd_msg_send(&req, pcrf_gx_raa_cb, svg);
    ogs_assert(ret == 0);
#endif

#if 0
    if (PccRuleList->count || QosDecisionList->count) {
        ogs_assert(true == pcf_sbi_send_smpolicycontrol_update_notify(
                            sess, &SmPolicyDecision));
    }
#endif 
    /* Increment the counter */
    ogs_assert(pthread_mutex_lock(&ogs_diam_logger_self()->stats_lock) == 0);
    ogs_diam_logger_self()->stats.nb_sent++;
    ogs_assert(pthread_mutex_unlock(&ogs_diam_logger_self()->stats_lock) == 0);

    /* Set no error */
    rx_message->result_code = ER_DIAMETER_SUCCESS;

    //ogs_session_data_free(&gx_message.session_data);

    return OGS_OK;

out:
    /* Store this value in the session */
    //ret = fd_sess_state_store(pcrf_gx_reg, session, &sess_data);
    //ogs_assert(sess_data == NULL);

    //ogs_session_data_free(&gx_message.session_data);
    
    return OGS_ERROR;
}