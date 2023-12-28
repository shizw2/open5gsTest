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

#include "sbi-path.h"
#include "ngap-path.h"
#include "nas-path.h"
#include "ngap-handler.h"
#include "nnrf-handler.h"
#include "namf-handler.h"
#include "nsmf-handler.h"
#include "nnssf-handler.h"
#include "nas-security.h"
#include "udp-ini-path.h"
#include "ngap-handler-sps.h"
#include "ogs-app-timer.h"
#include "license.h"

extern int g_sps_id;
extern pkt_fwd_tbl_t *g_pt_pkt_fwd_tbl;
extern int send_heart_cnt;

void amf_state_initial(ogs_fsm_t *s, amf_event_t *e)
{
    amf_sm_debug(e);

    ogs_assert(s);

    OGS_FSM_TRAN(s, &amf_state_operational);
}

void amf_state_final(ogs_fsm_t *s, amf_event_t *e)
{
    amf_sm_debug(e);

    ogs_assert(s);
}

void amf_state_operational(ogs_fsm_t *s, amf_event_t *e)
{
    int r, rv;
    char buf[OGS_ADDRSTRLEN];
    const char *api_version = NULL;

    ogs_sock_t *sock = NULL;
    ogs_sockaddr_t *addr = NULL;
    amf_gnb_t *gnb = NULL;
    uint16_t max_num_of_ostreams = 0;

    ogs_ngap_message_t ngap_message;
    ogs_pkbuf_t *pkbuf = NULL;
    int rc;

    ogs_nas_5gs_message_t nas_message;
    ran_ue_t *ran_ue = NULL;
    amf_ue_t *amf_ue = NULL;
    amf_sess_t *sess = NULL;

    ogs_sbi_object_t *sbi_object = NULL;
    ogs_sbi_xact_t *sbi_xact = NULL;
    int state = AMF_CREATE_SM_CONTEXT_NO_STATE;
    ogs_sbi_stream_t *stream = NULL;
    ogs_sbi_request_t *sbi_request = NULL;
    ogs_sbi_service_type_e service_type = OGS_SBI_SERVICE_TYPE_NULL;

    ogs_sbi_nf_instance_t *nf_instance = NULL;
    ogs_sbi_subscription_data_t *subscription_data = NULL;
    ogs_sbi_response_t *sbi_response = NULL;
    ogs_sbi_message_t sbi_message;

    char *supi = NULL;
    uint8_t sps_id = 0;
    amf_internel_msg_header_t internel_msg;
    ssize_t sent;
    int license_state;
    int bLoop;

    
    
    amf_sm_debug(e);

    ogs_assert(s);

    switch (e->h.id) {
    case OGS_FSM_ENTRY_SIG:
        break;

    case OGS_FSM_EXIT_SIG:
        break;

    case OGS_EVENT_SBI_SERVER:
        sbi_request = e->h.sbi.request;
        ogs_assert(sbi_request);
        stream = e->h.sbi.data;
        ogs_assert(stream);

        rv = ogs_sbi_parse_request(&sbi_message, sbi_request);
        if (rv != OGS_OK) {
            /* 'sbi_message' buffer is released in ogs_sbi_parse_request() */
            ogs_error("cannot parse HTTP sbi_message");
            ogs_assert(true ==
                ogs_sbi_server_send_error(
                    stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                    NULL, "cannot parse HTTP sbi_message", NULL));
            break;
        }

        sbi_message.udp_h.stream_pointer = stream;

        SWITCH(sbi_message.h.service.name)
        CASE(OGS_SBI_SERVICE_NAME_NUDM_SDM)
            api_version = OGS_SBI_API_V2;
            break;
        DEFAULT
            api_version = OGS_SBI_API_V1;
        END

        ogs_assert(api_version);
        if (strcmp(sbi_message.h.api.version, api_version) != 0) {
            ogs_error("Not supported version [%s]", sbi_message.h.api.version);
            ogs_assert(true ==
                ogs_sbi_server_send_error(
                    stream, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                    &sbi_message, "Not supported version", NULL));
            ogs_sbi_message_free(&sbi_message);
            break;
        }

        ogs_info("test:amf sbi server, service name:%s.",sbi_message.h.service.name);

        SWITCH(sbi_message.h.service.name)
        CASE(OGS_SBI_SERVICE_NAME_NNRF_NFM)

            SWITCH(sbi_message.h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_NF_STATUS_NOTIFY)
                SWITCH(sbi_message.h.method)
                CASE(OGS_SBI_HTTP_METHOD_POST)
                    if (is_amf_icps()){
                        //发给sps,sps更新nf信息
                        sbi_message.udp_h.isICPS = false;
                        for (bLoop = 0; bLoop < g_pt_pkt_fwd_tbl->b_sps_num; bLoop++){
                           udp_ini_msg_sendto(INTERNEL_MSG_SBI, &sbi_message.udp_h, sbi_request->http.content,sbi_request->http.content_length,g_pt_pkt_fwd_tbl->ta_sps_infos[bLoop].module_no);
                        }
                        
                        //icps 负责返回应答
                        sbi_message.udp_h.isICPS = true;
                        ogs_nnrf_nfm_handle_nf_status_notify(stream, &sbi_message);                    
                    }else{
                        ogs_nnrf_nfm_handle_nf_status_notify(stream, &sbi_message);
                    }            
                    break;

                DEFAULT
                    ogs_error("Invalid HTTP method [%s]", sbi_message.h.method);
                    ogs_assert(true ==
                        ogs_sbi_server_send_error(stream,
                            OGS_SBI_HTTP_STATUS_FORBIDDEN, &sbi_message,
                            "Invalid HTTP method", sbi_message.h.method));
                END
                break;

            DEFAULT
                ogs_error("Invalid resource name [%s]",
                        sbi_message.h.resource.component[0]);
                ogs_assert(true ==
                    ogs_sbi_server_send_error(stream,
                        OGS_SBI_HTTP_STATUS_BAD_REQUEST, &sbi_message,
                        "Invalid resource name",
                        sbi_message.h.resource.component[0]));
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NAMF_COMM)
            SWITCH(sbi_message.h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_UE_CONTEXTS)
                SWITCH(sbi_message.h.resource.component[2])
                CASE(OGS_SBI_RESOURCE_NAME_N1_N2_MESSAGES)
                    SWITCH(sbi_message.h.method)
                    CASE(OGS_SBI_HTTP_METHOD_POST)
                        if (is_amf_icps())
                        {
                            //获取supi,找到sps模块
                            supi = sbi_message.h.resource.component[1];
                            
                            /*
                            if (!ran_ue) {
                                ogs_error("No UE context [%s]", supi);
                                break;
                            }*/
                            /*
                            char *supi = sbi_message.h.resource.component[1];
                            sps_id = amf_sps_id_find_by_supi(supi);
                            if (0 == sps_id){
                                ogs_info("can not find sps id by supi %s, set sps_id as %d temporary,supilen:%ld.",supi,g_pt_pkt_fwd_tbl->ta_sps_infos[0].module_no,strlen(supi));
                                amf_sps_id_set_supi((uint8_t*)&g_pt_pkt_fwd_tbl->ta_sps_infos[0].module_no,supi);

                                sps_id = g_pt_pkt_fwd_tbl->ta_sps_infos[0].module_no;
                            }else{
                                ogs_info("find sps id %d by supi %s.",sps_id,supi);
                            } 
                            */    
                            //sps_id = ran_ue->sps_no;
                            sps_id=spsid_find_by_supi(sbi_message.h.resource.component[1]);
                            if (sps_id == 0 || sps_id > MAX_SPS_NUM){
                                ogs_error("sps id %d is invalid.",sps_id);                                
                                break;
                            }                 
                            ogs_info("stream addr:%p, stream_pointer:%ld,supi:%s,sps_id:%d.",stream, sbi_message.udp_h.stream_pointer,supi,sps_id);
                            udp_ini_msg_sendto(INTERNEL_MSG_SBI, &sbi_message.udp_h, sbi_request->http.content,sbi_request->http.content_length,sps_id);
                        }else{
                        rv = amf_namf_comm_handle_n1_n2_message_transfer(
                                stream, &sbi_message);
                        if (rv != OGS_OK) {
                            ogs_assert(true ==
                                ogs_sbi_server_send_error(stream,
                                    OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                                    &sbi_message,
                                    "No N1N2MessageTransferReqData", NULL));
                            }
                        }
                        break;

                    DEFAULT
                        ogs_error("Invalid HTTP method [%s]",
                                sbi_message.h.method);
                        ogs_assert(true ==
                            ogs_sbi_server_send_error(stream,
                                OGS_SBI_HTTP_STATUS_FORBIDDEN, &sbi_message,
                                "Invalid HTTP method", sbi_message.h.method));
                    END
                    break;

                DEFAULT
                    ogs_error("Invalid resource name [%s]",
                            sbi_message.h.resource.component[2]);
                    ogs_assert(true ==
                        ogs_sbi_server_send_error(stream,
                            OGS_SBI_HTTP_STATUS_BAD_REQUEST, &sbi_message,
                            "Invalid resource name",
                            sbi_message.h.resource.component[2]));
                END
                break;

            DEFAULT
                ogs_error("Invalid resource name [%s]",
                        sbi_message.h.resource.component[0]);
                ogs_assert(true ==
                    ogs_sbi_server_send_error(stream,
                        OGS_SBI_HTTP_STATUS_BAD_REQUEST, &sbi_message,
                        "Invalid resource name",
                        sbi_message.h.resource.component[0]));
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NAMF_CALLBACK)
            SWITCH(sbi_message.h.resource.component[1])
            CASE(OGS_SBI_RESOURCE_NAME_SM_CONTEXT_STATUS)
                if (is_amf_icps()){
                    sps_id=spsid_find_by_supi(sbi_message.h.resource.component[0]);
                    if (sps_id == 0 || sps_id > MAX_SPS_NUM){
                        ogs_error("sps id %d is invalid.",sps_id);                                
                        break;
                    }   
                    udp_ini_msg_sendto(INTERNEL_MSG_SBI, &sbi_message.udp_h, sbi_request->http.content,sbi_request->http.content_length,sps_id);
                }else{
                    amf_namf_callback_handle_sm_context_status(
                        stream, &sbi_message);
                }
                break;

            CASE(OGS_SBI_RESOURCE_NAME_DEREG_NOTIFY)
                if (is_amf_icps()){
                    sps_id=spsid_find_by_supi(sbi_message.h.resource.component[0]);
                    if (sps_id == 0 || sps_id > MAX_SPS_NUM){
                        ogs_error("sps id %d is invalid.",sps_id);                                
                        break;
                    }                    
                    udp_ini_msg_sendto(INTERNEL_MSG_SBI, &sbi_message.udp_h, sbi_request->http.content,sbi_request->http.content_length,sps_id);
                }else{
                    amf_namf_callback_handle_dereg_notify(stream, &sbi_message);
                }
                break;

            CASE(OGS_SBI_RESOURCE_NAME_SDMSUBSCRIPTION_NOTIFY)
                if (is_amf_icps()){
                    ogs_error("todo:OGS_SBI_RESOURCE_NAME_SDMSUBSCRIPTION_NOTIFY");
                    udp_ini_msg_sendto(INTERNEL_MSG_SBI, &sbi_message.udp_h, sbi_request->http.content,sbi_request->http.content_length,1);
                }else{
                    amf_namf_callback_handle_sdm_data_change_notify(
                        stream, &sbi_message);
                }
                break;

            CASE(OGS_SBI_RESOURCE_NAME_AM_POLICY_NOTIFY)
                ogs_assert(true == ogs_sbi_send_http_status_no_content(stream));
                break;

            DEFAULT
                ogs_error("Invalid resource name [%s]",
                        sbi_message.h.resource.component[1]);
                ogs_assert(true ==
                    ogs_sbi_server_send_error(stream,
                        OGS_SBI_HTTP_STATUS_BAD_REQUEST, &sbi_message,
                        "Invalid resource name",
                        sbi_message.h.resource.component[1]));
            END
            break;

        DEFAULT
            ogs_error("Invalid API name [%s]", sbi_message.h.service.name);
            ogs_assert(true ==
                ogs_sbi_server_send_error(stream,
                    OGS_SBI_HTTP_STATUS_BAD_REQUEST, &sbi_message,
                    "Invalid API name", sbi_message.h.resource.component[0]));
        END

        /* In lib/sbi/server.c, notify_completed() releases 'request' buffer. */
        ogs_sbi_message_free(&sbi_message);
        break;

    case OGS_EVENT_SBI_CLIENT:
        ogs_assert(e);

        sbi_response = e->h.sbi.response;
        ogs_assert(sbi_response);
        rv = ogs_sbi_parse_response(&sbi_message, sbi_response);
        if (rv != OGS_OK) {
            ogs_error("cannot parse HTTP response");
            ogs_sbi_message_free(&sbi_message);
            ogs_sbi_response_free(sbi_response);
            break;
        }

        SWITCH(sbi_message.h.service.name)
        CASE(OGS_SBI_SERVICE_NAME_NUDM_SDM)
        CASE(OGS_SBI_SERVICE_NAME_NNSSF_NSSELECTION)
            api_version = OGS_SBI_API_V2;
            break;
        DEFAULT
            api_version = OGS_SBI_API_V1;
        END

        ogs_assert(api_version);
        if (strcmp(sbi_message.h.api.version, api_version) != 0) {
            ogs_error("Not supported version [%s]", sbi_message.h.api.version);
            ogs_sbi_message_free(&sbi_message);
            ogs_sbi_response_free(sbi_response);
            break;
        }

        SWITCH(sbi_message.h.service.name)
        CASE(OGS_SBI_SERVICE_NAME_NNRF_NFM)

            SWITCH(sbi_message.h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_NF_INSTANCES)
                nf_instance = e->h.sbi.data;
                ogs_assert(nf_instance);
                ogs_assert(OGS_FSM_STATE(&nf_instance->sm));

                e->h.sbi.message = &sbi_message;
                ogs_fsm_dispatch(&nf_instance->sm, e);
                break;

            CASE(OGS_SBI_RESOURCE_NAME_SUBSCRIPTIONS)
                subscription_data = e->h.sbi.data;
                ogs_assert(subscription_data);

                SWITCH(sbi_message.h.method)
                CASE(OGS_SBI_HTTP_METHOD_POST)
                    if (sbi_message.res_status == OGS_SBI_HTTP_STATUS_CREATED ||
                        sbi_message.res_status == OGS_SBI_HTTP_STATUS_OK) {
                        ogs_nnrf_nfm_handle_nf_status_subscribe(
                                subscription_data, &sbi_message);
                    } else {
                        ogs_error("HTTP response error : %d",
                                sbi_message.res_status);
                    }
                    break;

                CASE(OGS_SBI_HTTP_METHOD_PATCH)
                    if (sbi_message.res_status == OGS_SBI_HTTP_STATUS_OK ||
                        sbi_message.res_status ==
                            OGS_SBI_HTTP_STATUS_NO_CONTENT) {
                        ogs_nnrf_nfm_handle_nf_status_update(
                                subscription_data, &sbi_message);
                    } else {
                        ogs_error("[%s] HTTP response error [%d]",
                                subscription_data->id ?
                                    subscription_data->id : "Unknown",
                                sbi_message.res_status);
                    }
                    break;

                CASE(OGS_SBI_HTTP_METHOD_DELETE)
                    if (sbi_message.res_status ==
                            OGS_SBI_HTTP_STATUS_NO_CONTENT) {
                        ogs_sbi_subscription_data_remove(subscription_data);
                    } else {
                        ogs_error("[%s] HTTP response error [%d]",
                                subscription_data->id ?
                                    subscription_data->id : "Unknown",
                                sbi_message.res_status);
                    }
                    break;

                DEFAULT
                    ogs_error("Invalid HTTP method [%s]", sbi_message.h.method);
                    ogs_assert_if_reached();
                END
                break;
            
            DEFAULT
                ogs_error("Invalid resource name [%s]",
                        sbi_message.h.resource.component[0]);
                ogs_assert_if_reached();
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NNRF_DISC)
            SWITCH(sbi_message.h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_NF_INSTANCES)
                sbi_xact = e->h.sbi.data;
                ogs_assert(sbi_xact);

                SWITCH(sbi_message.h.method)
                CASE(OGS_SBI_HTTP_METHOD_GET)
                    if (sbi_message.res_status == OGS_SBI_HTTP_STATUS_OK)
                        amf_nnrf_handle_nf_discover(sbi_xact, &sbi_message);
                    else
                        ogs_error("HTTP response error [%d]",
                                sbi_message.res_status);
                    break;

                DEFAULT
                    ogs_error("Invalid HTTP method [%s]", sbi_message.h.method);
                    ogs_assert_if_reached();
                END
                break;

            DEFAULT
                ogs_error("Invalid resource name [%s]",
                        sbi_message.h.resource.component[0]);
                ogs_assert_if_reached();
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NAUSF_AUTH)
        CASE(OGS_SBI_SERVICE_NAME_NUDM_UECM)
        CASE(OGS_SBI_SERVICE_NAME_NUDM_SDM)
        CASE(OGS_SBI_SERVICE_NAME_NPCF_AM_POLICY_CONTROL)            
            if (is_amf_icps()){
                ogs_info("test icps:sbi_message.h.service.name:%s.",sbi_message.h.service.name);
                udp_ini_msg_sendto(INTERNEL_MSG_SBI, &sbi_message.udp_h, sbi_response->http.content,sbi_response->http.content_length,1);
            }else{
                ogs_info("test sps:sbi_message.h.service.name:%s.",sbi_message.h.service.name);
                //udp_ini_msg_sendto_icps(INTERNEL_MSG_SBI, &sbi_message.udp_h, sbi_response->http.content,sbi_response->http.content_length);
            }
            sbi_xact = e->h.sbi.data;
            ogs_assert(sbi_xact);

            sbi_xact = ogs_sbi_xact_cycle(sbi_xact);
            if (!sbi_xact) {
                /* CLIENT_WAIT timer could remove SBI transaction
                 * before receiving SBI message */
                ogs_error("SBI transaction has already been removed");
                break;
            }

            state = sbi_xact->state;

            amf_ue = (amf_ue_t *)sbi_xact->sbi_object;
            ogs_assert(amf_ue);

            ogs_sbi_xact_remove(sbi_xact);

            amf_ue = amf_ue_cycle(amf_ue);
            if (!amf_ue) {
                ogs_error("UE(amf_ue) Context has already been removed");
                break;
            }

            ogs_assert(OGS_FSM_STATE(&amf_ue->sm));

            e->amf_ue = amf_ue;
            e->h.sbi.message = &sbi_message;;
            e->h.sbi.state = state;

            ogs_fsm_dispatch(&amf_ue->sm, e);
            break;

        CASE(OGS_SBI_SERVICE_NAME_NSMF_PDUSESSION)
            sbi_xact = e->h.sbi.data;
            ogs_assert(sbi_xact);

            sbi_xact = ogs_sbi_xact_cycle(sbi_xact);
            if (!sbi_xact) {
                /* CLIENT_WAIT timer could remove SBI transaction
                 * before receiving SBI message */
                ogs_error("SBI transaction has already been removed");
                break;
            }

            state = sbi_xact->state;

            sess = (amf_sess_t *)sbi_xact->sbi_object;
            ogs_assert(sess);

            ogs_sbi_xact_remove(sbi_xact);

            sess = amf_sess_cycle(sess);
            if (!sess) {
            /*
             * 1. If AMF-UE context is duplicated in Identity-Response,
             *    OLD AMF-UE's all session contexts are removed.
             * 2. If there is an unfinished transaction with SMF,
             *    Transaction's session context is NULL.
             *
             * For example,
             *
             * 1. gNB->AMF : PDUSessionResourceSetupResponse
             * 2. AMF->SMF : [POST] /nsmf-pdusession/v1/sm-contexts/1/modify
             * 3. UE ->AMF : Registration request with Unknwon GUTI
             * 4. AMF->UE  : Identity request
             * 5. UE ->AMF : Identity response
             *               AMF UE context duplicated.
             *               All session contexts are removed
             * 6. SMF->AMF : RESPONSE /nsmf-pdusession/v1/sm-contexts/1/modify
             *               No Session Context
             *               Assertion
             *
             * OR
             *
             * In ./tests/vonr/af-test/test3_func()
             * 1. Send PDU session establishment request
             * 2. Receive PDU session establishment accept
             * 3. Send PDUSessionResourceSetupResponse
             * 4. Send De-registration request
             * 5. SMF->AMF : RESPONSE /nsmf-pdusession/v1/sm-contexts/3/release
             * 6. SMF->AMF : RESPONSE /nsmf-pdusession/v1/sm-contexts/1/modify
             *
             * IF THIS HAPPENS IN THE REAL WORLD,
             * I WILL MODIFY THE ASSERTS BELOW.
             */
                ogs_error("Session has already been removed");
                break;
            }

            amf_ue = sess->amf_ue;
            ogs_assert(amf_ue);
            amf_ue = amf_ue_cycle(amf_ue);
            ogs_assert(amf_ue);

            ogs_assert(OGS_FSM_STATE(&amf_ue->sm));

            e->amf_ue = amf_ue;
            e->sess = sess;
            e->h.sbi.message = &sbi_message;;

            SWITCH(sbi_message.h.resource.component[2])
            CASE(OGS_SBI_RESOURCE_NAME_MODIFY)
                rv = amf_nsmf_pdusession_handle_update_sm_context(
                        sess, state, &sbi_message);
                if (rv != OGS_OK) {
                    AMF_SESS_CLEAR(sess);
                }
                break;

            CASE(OGS_SBI_RESOURCE_NAME_RELEASE)
                if (sbi_message.res_status == OGS_SBI_HTTP_STATUS_NO_CONTENT ||
                    sbi_message.res_status == OGS_SBI_HTTP_STATUS_OK) {
                    ogs_info("[%s:%d] Release SM context [%d]",
                            amf_ue->supi, sess->psi, sbi_message.res_status);
                } else {
                    ogs_error("[%s] HTTP response error [%d]",
                            amf_ue->supi, sbi_message.res_status);
                }

                amf_nsmf_pdusession_handle_release_sm_context(sess, state);
                break;

            DEFAULT
                rv = amf_nsmf_pdusession_handle_create_sm_context(
                        sess, &sbi_message);
                if (rv != OGS_OK) {
                    /*
                     * 1. First PDU session establishment request
                     *    (PSI:5, internet)
                     * 2. First session created
                     * 3. Seconds PDU session establishment request
                     *    (PSI:5, ims)
                     * 4. AMF sends DUPLICATED_PDU_SESSION_ID to the SMF
                     * 5. AMF try to create second PDU session.
                     * 6. But, Second session rejected due to Subscription Info.
                     *
                     * In above situation, AMF need to clear SM_CONTEXT_REF.
                     * Otherwise, AMF have redundant PDU session.
                     *
                     * Moreover, AMF could send UEContextReleaseRequest
                     * with deactivating this redundant session.
                     *
                     * So, if CreateSMContext is failed,
                     * we'll clear SM_CONTEXT_REF.
                     */
                    AMF_SESS_CLEAR(sess);
                }
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NNSSF_NSSELECTION)
            sbi_xact = e->h.sbi.data;
            ogs_assert(sbi_xact);

            sbi_xact = ogs_sbi_xact_cycle(sbi_xact);
            if (!sbi_xact) {
                /* CLIENT_WAIT timer could remove SBI transaction
                 * before receiving SBI message */
                ogs_error("SBI transaction has already been removed");
                break;
            }

            sess = (amf_sess_t *)sbi_xact->sbi_object;
            ogs_assert(sess);

            state = sbi_xact->state;

            ogs_sbi_xact_remove(sbi_xact);

            sess = amf_sess_cycle(sess);
            if (!sess) {
                ogs_error("Session has already been removed");
                break;
            }

            amf_ue = sess->amf_ue;
            ogs_assert(amf_ue);
            amf_ue = amf_ue_cycle(amf_ue);
            ogs_assert(amf_ue);

            ogs_assert(OGS_FSM_STATE(&amf_ue->sm));

            e->amf_ue = amf_ue;
            e->sess = sess;
            e->h.sbi.message = &sbi_message;;
            e->h.sbi.state = state;

            amf_nnssf_nsselection_handle_get(sess, &sbi_message);
            break;

        DEFAULT
            ogs_error("Invalid service name [%s]", sbi_message.h.service.name);
            ogs_assert_if_reached();
        END

        ogs_sbi_message_free(&sbi_message);
        ogs_sbi_response_free(sbi_response);
        break;

    case OGS_EVENT_SBI_TIMER:
        ogs_assert(e);

        switch(e->h.timer_id) {
        case OGS_TIMER_NF_INSTANCE_REGISTRATION_INTERVAL:
        case OGS_TIMER_NF_INSTANCE_HEARTBEAT_INTERVAL:
        case OGS_TIMER_NF_INSTANCE_NO_HEARTBEAT:
        case OGS_TIMER_NF_INSTANCE_VALIDITY:
            nf_instance = e->h.sbi.data;
            ogs_assert(nf_instance);
            ogs_assert(OGS_FSM_STATE(&nf_instance->sm));

            ogs_sbi_self()->nf_instance->load = amf_instance_get_load();

            ogs_fsm_dispatch(&nf_instance->sm, e);
            if (OGS_FSM_CHECK(&nf_instance->sm, ogs_sbi_nf_state_exception))
                ogs_error("[%s:%s] State machine exception [%d]",
                        OpenAPI_nf_type_ToString(nf_instance->nf_type),
                        nf_instance->id, e->h.timer_id);
            break;

        case OGS_TIMER_SUBSCRIPTION_VALIDITY:
            subscription_data = e->h.sbi.data;
            ogs_assert(subscription_data);

            ogs_assert(true ==
                ogs_nnrf_nfm_send_nf_status_subscribe(
                    ogs_sbi_self()->nf_instance->nf_type,
                    subscription_data->req_nf_instance_id,
                    subscription_data->subscr_cond.nf_type,
                    subscription_data->subscr_cond.service_name));

            ogs_error("[%s] Subscription validity expired",
                subscription_data->id);
            ogs_sbi_subscription_data_remove(subscription_data);
            break;

        case OGS_TIMER_SUBSCRIPTION_PATCH:
            subscription_data = e->h.sbi.data;
            ogs_assert(subscription_data);

            ogs_assert(true ==
                ogs_nnrf_nfm_send_nf_status_update(subscription_data));

            ogs_info("[%s] Need to update Subscription",
                    subscription_data->id);
            break;

        case OGS_TIMER_SBI_CLIENT_WAIT:
            /*
             * ogs_pollset_poll() receives the time of the expiration
             * of next timer as an argument. If this timeout is
             * in very near future (1 millisecond), and if there are
             * multiple events that need to be processed by ogs_pollset_poll(),
             * these could take more than 1 millisecond for processing,
             * resulting in the timer already passed the expiration.
             *
             * In case that another NF is under heavy load and responds
             * to an SBI request with some delay of a few seconds,
             * it can happen that ogs_pollset_poll() adds SBI responses
             * to the event list for further processing,
             * then ogs_timer_mgr_expire() is called which will add
             * an additional event for timer expiration. When all events are
             * processed one-by-one, the SBI xact would get deleted twice
             * in a row, resulting in a crash.
             *
             * 1. ogs_pollset_poll()
             *    message was received and put into an event list,
             * 2. ogs_timer_mgr_expire()
             *    add an additional event for timer expiration
             * 3. message event is processed. (free SBI xact)
             * 4. timer expiration event is processed. (double-free SBI xact)
             *
             * To avoid double-free SBI xact,
             * we need to check ogs_sbi_xact_cycle()
             */
            sbi_xact = ogs_sbi_xact_cycle(e->h.sbi.data);
            if (!sbi_xact) {
                ogs_error("SBI transaction has already been removed");
                break;
            }

            sbi_object = sbi_xact->sbi_object;
            ogs_assert(sbi_object);

            service_type = sbi_xact->service_type;

            ogs_sbi_xact_remove(sbi_xact);

            ogs_assert(sbi_object->type > OGS_SBI_OBJ_BASE &&
                        sbi_object->type < OGS_SBI_OBJ_TOP);

            switch(sbi_object->type) {
            case OGS_SBI_OBJ_UE_TYPE:
                amf_ue = (amf_ue_t *)sbi_object;
                ogs_assert(amf_ue);
                amf_ue = amf_ue_cycle(amf_ue);
                if (!amf_ue) {
                    ogs_error("UE(amf_ue) Context has already been removed");
                    break;
                }

                ogs_error("[%s] Cannot receive SBI message", amf_ue->suci);
                r = nas_5gs_send_gmm_reject_from_sbi(amf_ue,
                        OGS_SBI_HTTP_STATUS_GATEWAY_TIMEOUT);
                ogs_expect(r == OGS_OK);
                ogs_assert(r != OGS_ERROR);
                break;

            case OGS_SBI_OBJ_SESS_TYPE:
                sess = (amf_sess_t *)sbi_object;
                ogs_assert(sess);
                sess = amf_sess_cycle(sess);
                if (!sess) {
                    ogs_error("Session has already been removed");
                    break;
                }

                amf_ue = sess->amf_ue;
                ogs_assert(amf_ue);
                amf_ue = amf_ue_cycle(amf_ue);
                if (!amf_ue) {
                    ogs_error("UE(amf_ue) Context has already been removed");
                    break;
                }

                ogs_error("[%d:%d] Cannot receive SBI message",
                        sess->psi, sess->pti);
                if (sess->payload_container_type) {
                    r = nas_5gs_send_back_gsm_message(sess,
                            OGS_5GMM_CAUSE_PAYLOAD_WAS_NOT_FORWARDED,
                            AMF_NAS_BACKOFF_TIME);
                    ogs_expect(r == OGS_OK);
                    ogs_assert(r != OGS_ERROR);
                } else {
                    r = ngap_send_error_indication2_sps(amf_ue,
                            NGAP_Cause_PR_transport,
                            NGAP_CauseTransport_transport_resource_unavailable);
                    ogs_expect(r == OGS_OK);
                    ogs_assert(r != OGS_ERROR);
                }
                break;

            default:
                ogs_fatal("Not implemented [%s:%d]",
                    ogs_sbi_service_type_to_name(service_type),
                    sbi_object->type);
                ogs_assert_if_reached();
            }
            break;

        default:
            ogs_error("Unknown timer[%s:%d]",
                    ogs_timer_get_name(e->h.timer_id), e->h.timer_id);
        }
        break;

    case OGS_EVENT_APP_CHECK_TIMER:
        ogs_assert(e);

        switch(e->h.timer_id) {
        case OGS_TIMER_LICENSE_CHECK:
            license_state = checkLicenseAfterRuntime(LICENSE_CHECK_INTERVAL,30);
            ogs_info("license state:%s, runtime:%lu, durationtime:%lu, expireTime:%s.", 
                    get_license_state_name(license_state),
                    getLicenseRunTime(),
                    getLicenseDurationTime(),
                    timestampToString(getLicenseExpireTime()));
            report_license_alarm(license_state);
            if (license_state == LICENSE_STATE_SOON_TO_EXPIRE){
                ogs_warn("license soon to expire.");                              
            }else if (license_state == LICENSE_STATE_EXPIRED){
                ogs_fatal("license expired.");
                exit(0);
            }

            license_check_restart();
            break;

        case OGS_TIMER_YAML_CONFIG_CHECK:
            yaml_check_proc();
            ogs_yaml_check_restart(); 
            break;

        default:
            ogs_error("Unknown timer[%s:%d]",
                    ogs_timer_get_name(e->h.timer_id), e->h.timer_id);
        }
        break;

    case AMF_EVENT_NGAP_LO_ACCEPT:
        sock = e->ngap.sock;
        ogs_assert(sock);
        addr = e->ngap.addr;
        ogs_assert(addr);

        ogs_info("gNB-N2 accepted[%s] in master_sm module",
            OGS_ADDR(addr, buf));

        gnb = amf_gnb_find_by_addr(addr);
        if (!gnb) {
            gnb = amf_gnb_add(sock, addr);
            ogs_assert(gnb);
        } else {
            ogs_warn("gNB context duplicated with IP-address [%s]!!!",
                    OGS_ADDR(addr, buf));
            ogs_sock_destroy(sock);
            ogs_free(addr);
            ogs_warn("N2 Socket Closed");
        }

        break;

    case AMF_EVENT_NGAP_LO_SCTP_COMM_UP:
        sock = e->ngap.sock;
        ogs_assert(sock);
        addr = e->ngap.addr;
        ogs_assert(addr);

        max_num_of_ostreams = e->ngap.max_num_of_ostreams;

        gnb = amf_gnb_find_by_addr(addr);
        if (!gnb) {
            gnb = amf_gnb_add(sock, addr);
            ogs_assert(gnb);
        } else {
            ogs_free(addr);
        }

        if (gnb->max_num_of_ostreams)
            gnb->max_num_of_ostreams =
                    ogs_min(max_num_of_ostreams, gnb->max_num_of_ostreams);
        else
            gnb->max_num_of_ostreams = max_num_of_ostreams;

        ogs_info("gNB-N2[%s] max_num_of_ostreams : %d",
            OGS_ADDR(gnb->sctp.addr, buf), gnb->max_num_of_ostreams);

        break;

    case AMF_EVENT_NGAP_LO_CONNREFUSED:
        sock = e->ngap.sock;
        ogs_assert(sock);
        addr = e->ngap.addr;
        ogs_assert(addr);

        gnb = amf_gnb_find_by_addr(addr);
        if (gnb) {
            ogs_info("gNB-N2[%s] connection refused!!!", OGS_ADDR(addr, buf));
            amf_sbi_send_deactivate_all_ue_in_gnb(
                    gnb, AMF_REMOVE_S1_CONTEXT_BY_LO_CONNREFUSED);
            amf_gnb_remove(gnb);
        } else {
            ogs_warn("gNB-N2[%s] connection refused, Already Removed!",
                    OGS_ADDR(addr, buf));
        }
        ogs_free(addr);

        break;
    case AMF_EVENT_NGAP_MESSAGE:
        sock = e->ngap.sock;
        ogs_assert(sock);
        addr = e->ngap.addr;
        ogs_assert(addr);
        pkbuf = e->pkbuf;
        ogs_assert(pkbuf);

        gnb = amf_gnb_find_by_addr(addr);
        ogs_free(addr);

        ogs_assert(gnb);
        ogs_assert(OGS_FSM_STATE(&gnb->sm));

        rc = ogs_ngap_decode(&ngap_message, pkbuf);
        if (rc == OGS_OK) {
            e->gnb = gnb;
            e->ngap.message = &ngap_message;
            ogs_fsm_dispatch(&gnb->sm, e);
        } else {
            ogs_error("Cannot decode NGAP message");
            r = ngap_send_error_indication(
                    gnb, NULL, NULL, NGAP_Cause_PR_protocol, 
                    NGAP_CauseProtocol_abstract_syntax_error_falsely_constructed_message);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
        }

        ogs_ngap_free(&ngap_message);
        ogs_pkbuf_free(pkbuf);
        break;

    case AMF_EVENT_NGAP_TIMER:
        ran_ue = e->ran_ue;
        ogs_assert(ran_ue);

        switch (e->h.timer_id) {
        case AMF_TIMER_NG_DELAYED_SEND:
            
            pkbuf = e->pkbuf;
            ogs_assert(pkbuf);
            if(is_amf_icps()){
                gnb = e->gnb;
                ogs_assert(gnb);
                r = ngap_send_to_ran_ue(ran_ue, pkbuf);
                ogs_expect(r == OGS_OK);
                ogs_assert(r != OGS_ERROR);
                }else{
                    ogs_expect(OGS_OK == ngap_send_to_ran_ue_sps(ran_ue, pkbuf));
                }
                ogs_timer_delete(e->timer);
            break;
        case AMF_TIMER_NG_HOLDING:
            ogs_warn("Implicit NG release");
            ogs_warn("    RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld]",
                  ran_ue->ran_ue_ngap_id,
                  (long long)ran_ue->amf_ue_ngap_id);
            ngap_handle_ue_context_release_action(ran_ue);
            break;
        default:
            ogs_error("Unknown timer[%s:%d]",
                    amf_timer_get_name(e->h.timer_id), e->h.timer_id);
            break;
        }
        break;

    case AMF_EVENT_5GMM_MESSAGE:
        ran_ue = e->ran_ue;
        ogs_assert(ran_ue);
        pkbuf = e->pkbuf;
        ogs_assert(pkbuf);

        if (ogs_nas_5gmm_decode(&nas_message, pkbuf) != OGS_OK) {
            ogs_error("ogs_nas_5gmm_decode() failed");
            ogs_pkbuf_free(pkbuf);
            break;
        }

        amf_ue = ran_ue->amf_ue;
        if (!amf_ue) {
            amf_ue = amf_ue_find_by_message(&nas_message);
            if (!amf_ue) {
                amf_ue = amf_ue_add(ran_ue);
                if (amf_ue == NULL) {
                    r = ngap_send_ran_ue_context_release_command(
                            ran_ue,
                            NGAP_Cause_PR_misc,
                            NGAP_CauseMisc_control_processing_overload,
                            NGAP_UE_CTX_REL_NG_CONTEXT_REMOVE, 0);
                    ogs_expect(r == OGS_OK);
                    ogs_assert(r != OGS_ERROR);
                    ogs_pkbuf_free(pkbuf);
                    break;
                }
            } else {
                /* Here, if the AMF_UE Context is found,
                 * the integrity check is not performed
                 * For example, REGISTRATION_REQUEST,
                 * TRACKING_AREA_UPDATE_REQUEST message
                 *
                 * Now, We will check the MAC in the NAS message*/
                ogs_nas_security_header_type_t h;
                h.type = e->nas.type;
                if (h.integrity_protected) {
                    /* Decryption was performed in NGAP handler.
                     * So, we disabled 'ciphered'
                     * not to decrypt NAS message */
                    h.ciphered = 0;
                    if (nas_5gs_security_decode(amf_ue, h, pkbuf) != OGS_OK) {
                        ogs_error("[%s] nas_security_decode() failed",
                                amf_ue->suci);
                        ogs_pkbuf_free(pkbuf);
                        break;
                    }
                }
            }

            /* 
             * TS23.502
             * 4.2.3.2 UE Triggered Service Request
             *
             * 4. [Conditional]
             * AMF to SMF: Nsmf_PDUSession_UpdateSMContext Request
             *
             * The AMF may receive a Service Request to establish another
             * NAS signalling connection via a NG-RAN while it has maintained
             * an old NAS signalling connection for UE still via NG-RAN.
             * In this case, AMF shall trigger the AN release procedure toward
             * the old NG-RAN to release the old NAS signalling connection
             * as defined in clause 4.2.6 with following logic: */

            /* If NAS(amf_ue_t) has already been associated with
             * older NG(ran_ue_t) context */
            if (CM_CONNECTED(amf_ue)) {
                /* Previous NG(ran_ue_t) context the holding timer(30secs)
                 * is started.
                 * Newly associated NG(ran_ue_t) context holding timer
                 * is stopped. */
                ogs_debug("[%s] Start NG Holding Timer", amf_ue->suci);
                ogs_debug("[%s]    RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld]",
                        amf_ue->suci, amf_ue->ran_ue->ran_ue_ngap_id,
                        (long long)amf_ue->ran_ue->amf_ue_ngap_id);

                /* De-associate NG with NAS/EMM */
                ran_ue_deassociate(amf_ue->ran_ue);

                r = ngap_send_ran_ue_context_release_command(amf_ue->ran_ue,
                        NGAP_Cause_PR_nas, NGAP_CauseNas_normal_release,
                        NGAP_UE_CTX_REL_NG_CONTEXT_REMOVE, 0);
                ogs_expect(r == OGS_OK);
                ogs_assert(r != OGS_ERROR);
            }
            amf_ue_associate_ran_ue(amf_ue, ran_ue);
            ogs_info("????????SPS AMF_EVENT_5GMM_MESSAGE amf_ue->ran_ue->amf_ue_ngap_id:%lu",amf_ue->ran_ue->amf_ue_ngap_id);
        }

        ogs_assert(amf_ue);
        ogs_assert(OGS_FSM_STATE(&amf_ue->sm));

        e->amf_ue = amf_ue;
        e->nas.message = &nas_message;

        ogs_fsm_dispatch(&amf_ue->sm, e);

        ogs_pkbuf_free(pkbuf);
        break;

    case AMF_EVENT_5GMM_TIMER:
        amf_ue = amf_ue_cycle(e->amf_ue);
        if (!amf_ue) {
            ogs_error("UE(amf_ue) Context has already been removed");
            break;
        }

        ogs_assert(OGS_FSM_STATE(&amf_ue->sm));

        ogs_fsm_dispatch(&amf_ue->sm, e);
        break;

    case AMF_EVENT_INTERNEL_TIMER:
		sock = e->internal_sock;
        ogs_assert(sock);

        switch (e->h.timer_id) {
        case AMF_TIMER_INTERNEL_HEARTBEAT:
            udp_ini_send_hand_shake();            
            sps_check_icps_offline();
            break;

        case AMF_TIMER_INTERNEL_HEARTBEAT_CHECK:            
            udp_ini_hand_shake_check();
            break;
        default:
            ogs_error("Unknown timer[%s:%d]",
                    amf_timer_get_name(e->h.timer_id), e->h.timer_id);
            break;
        }
        break;

    case AMF_EVENT_INTERNEL_MESSAGE:
		pkbuf = e->pkbuf;
		amf_internel_msg_header_t *pmsg = (amf_internel_msg_header_t *)pkbuf->data;
		if (is_amf_icps())
		{
            switch (pmsg->msg_type)
            {
                case INTERNEL_MSG_HAND_SHAKE_REQ:
                {
                    udp_ini_icps_handle_hand_shake(pmsg);
                    break;
                }
                case INTERNEL_MSG_SUPI_NOTIFY:
                {
                    udp_ini_icps_handle_supi_notify(pkbuf);
                    break;
                }
                case INTERNEL_MSG_SUPI_HASH_REMOVE_NOTIFY:
                {
                    udp_ini_icps_handle_supi_hash_remove_notify(pkbuf);
                    break;
                }
                case  INTERNEL_MSG_NGAP:
                {
                    //TODO
                   
					int rv;
				    rv=icps_handle_rev_ini_ngap(pmsg,pkbuf);
				    if(rv==OGS_OK)
				   	 ogs_info(" ICPS rev INTERNEL_MSG_NGAP,ICPS Send TO Ngap OK! ");
                    break;
                }
                case  INTERNEL_MSG_SBI:
                {
                    ogs_info("icps recv sbi msg.");
                    udp_ini_icps_handle_sbi_msg(pkbuf);
                    break;
                }
                default:
                    ogs_error("icps unknown msg, msgtype:%d.",pmsg->msg_type);
            }
		}
		else
		{
            switch (pmsg->msg_type)
            {
                case INTERNEL_MSG_HAND_SHAKE_RSP:
                {
                    
                    send_heart_cnt=0;
                    //ogs_info("sps recv internel msg handshake rsp from icps,msg_type:%d,sps_id:%d,state:%d，send_heart_cnt：%d.",pmsg->msg_type,pmsg->sps_id,pmsg->sps_state,send_heart_cnt);

                    break;
                }
                case  INTERNEL_MSG_NGAP:
                {
                    //TODO
				   
				   int rv;
				   rv=sps_handle_rev_ini_ngap(pmsg,pkbuf);
				   if(rv==OGS_OK)
				   	ogs_info(" SPS rev INTERNEL_MSG_NGAP,ICPS Send SPS OK! ");
                    break;
                }
                case  INTERNEL_MSG_SBI:
                {					
					udp_ini_handle_sbi_msg(pkbuf);
                    break;	
                }
                default:
                    ogs_error("sps unknown msg, msgtype:%d.",pmsg->msg_type);
            }			
		}

        if (is_amf_icps()&& pmsg->msg_type == INTERNEL_MSG_NGAP){
            //NGAP的在sctp发送后自行释放,这里不需要再次释放
        }else{
            ogs_pkbuf_free(pkbuf);
        }
        break;
    default:
        ogs_error("No handler for event %s", amf_event_get_name(e));
        break;
    }
}

bool is_amf_icps(void)
{
	if (g_sps_id == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool is_amf_sps(void)
{
	if (g_sps_id != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}