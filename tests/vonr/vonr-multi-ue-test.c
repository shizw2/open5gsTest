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

#include "test-common.h"
#include <stdio.h>	//使用printf()需要包含
#include <sys/time.h>
#include <unistd.h>	//使用sleep()函数需要包含
#include <time.h>


#include "test-fd-path.h"

#include "af/sbi-path.h"


extern int g_testNum;
extern int g_threadNum;

static ogs_thread_t *mthread3;
extern int g_testcycleNum;

typedef struct threadinfo
{
    abts_case   *tc;
    int         clientIdx ;	
}T_threadinfo3;

int iPthreadnum3 = 3;
void muti_ue_func31(void *data);
void muti_ue_func32(void *data);
void muti_ue_func33(void *data);
void muti_ue_func34(void *data);
void muti_ue_func35(void *data);
void muti_ue_func36(void *data);
void muti_ue_func37(void *data);



test_ue_t *test_ues3[MAX_THREAD][10000];
ogs_socknode_t *ngaps[MAX_THREAD];
ogs_socknode_t *gtpus[2];

const struct testcaselist3 {
    void *(*func)(void *data);
} casetests3[] = {
    {muti_ue_func31},
    {muti_ue_func35},
    {muti_ue_func33},
    {muti_ue_func34},
    {muti_ue_func35},
    {muti_ue_func36},
    {muti_ue_func37},
    {NULL},
};


static void muti_ue_threads(abts_case *tc, void *data)
{
#if 1
    int iTmp;
    ogs_thread_t *id[MAX_THREAD];
    T_threadinfo3 threadInfo[MAX_THREAD];
	ogs_nas_5gs_mobile_identity_suci_t mobile_identity_suci;
    //test_ue_t *test_ue[MAX_THREAD][g_testNum];
	int i,rv;
	bson_t *doc = NULL;
	test_sess_t *sess = NULL;
	ogs_pkbuf_t *sendbuf;
    ogs_pkbuf_t *recvbuf; 
    uint32_t gnb_id;
    gnb_id=0x4000;
	iPthreadnum3 = g_threadNum;
	if  (g_threadNum > 7)
	{
		iPthreadnum3 = 7;
        printf("MAX thread num is 4!\n");
	}
	
    /* Two gNB connects to UPF */
    gtpus[0] = test_gtpu_server(1, AF_INET);
    ABTS_PTR_NOTNULL(tc, gtpus[0]);

    gtpus[1] = test_gtpu_server(2, AF_INET);
    ABTS_PTR_NOTNULL(tc, gtpus[1]);

	
	for (iTmp = 0; iTmp < iPthreadnum3; iTmp++)
	{
		for (i = 0; i < g_testNum; i++) {
			uint64_t imsi_index;

			/* Setup Test UE & Session Context */
			memset(&mobile_identity_suci, 0, sizeof(mobile_identity_suci));

			mobile_identity_suci.h.supi_format = OGS_NAS_5GS_SUPI_FORMAT_IMSI;
			mobile_identity_suci.h.type = OGS_NAS_5GS_MOBILE_IDENTITY_SUCI;
			mobile_identity_suci.routing_indicator1 = 0;
			mobile_identity_suci.routing_indicator2 = 0xf;
			mobile_identity_suci.routing_indicator3 = 0xf;
			mobile_identity_suci.routing_indicator4 = 0xf;
			mobile_identity_suci.protection_scheme_id = OGS_NAS_5GS_NULL_SCHEME;
			mobile_identity_suci.home_network_pki_value = 0;
			mobile_identity_suci.scheme_output[0] = 0;
			mobile_identity_suci.scheme_output[1] = 0;
			mobile_identity_suci.scheme_output[2] = 0x20;
			mobile_identity_suci.scheme_output[3] = 0x31;
			mobile_identity_suci.scheme_output[4] = 0x90;

			imsi_index = iTmp*10000+ i + 1;
			//printf("imsi:%lu.\r\n",imsi_index);
			//ogs_uint64_to_buffer(imsi_index, 5, mobile_identity_suci.scheme_output);
			mobile_identity_suci.scheme_output[0] = imsi_index/10000%10;
			mobile_identity_suci.scheme_output[1] = imsi_index/1000%10;
			mobile_identity_suci.scheme_output[2] = imsi_index/100%10;
			mobile_identity_suci.scheme_output[3] = imsi_index/10%10;
			mobile_identity_suci.scheme_output[4] = imsi_index%10;

			test_ues3[iTmp][i] = test_ue_add_by_suci(&mobile_identity_suci, 13);
			ogs_assert(test_ues3[iTmp][i]);

			test_ues3[iTmp][i]->nr_cgi.cell_id = 0x40001;

			test_ues3[iTmp][i]->nas.registration.tsc = 0;
			test_ues3[iTmp][i]->nas.registration.ksi = OGS_NAS_KSI_NO_KEY_IS_AVAILABLE;
			test_ues3[iTmp][i]->nas.registration.follow_on_request = 1;
			test_ues3[iTmp][i]->nas.registration.value = OGS_NAS_5GS_REGISTRATION_TYPE_INITIAL;

			test_ues3[iTmp][i]->k_string = "465b5ce8b199b49faa5f0a2ee238a6bc";
			test_ues3[iTmp][i]->opc_string = "e8ed289deba952e4283b54e88e6183ca";
		}


		//插入数据库单独统计
		for (i = 0; i < g_testNum; i++) {
#if 0
			/* Send PDU session establishment request */
			sess = test_sess_add_by_dnn_and_psi(test_ues[iTmp][i], "internet", 5);
			ogs_assert(sess);
#endif		
			/********** Insert Subscriber in Database */
			//
			if(iTmp==3)
                doc = test_db_new_session(test_ues3[iTmp][i]);
            else if(iTmp==0)
                doc = test_db_new_qos_flow(test_ues3[iTmp][i]);
            else
            doc = test_db_new_ims(test_ues3[iTmp][i]);      

			ABTS_PTR_NOTNULL(tc, doc);
			ABTS_INT_EQUAL(tc, OGS_OK, test_db_insert_ue(test_ues3[iTmp][i], doc));
		}
    }

	for (iTmp = 0; iTmp < iPthreadnum3; iTmp++)
	{        
        for (i = 0; i < g_testNum; i++) {

            /* Send PDU session establishment request */
            sess = test_sess_add_by_dnn_and_psi(test_ues3[iTmp][i], "internet", 5);
            ogs_assert(sess);
            if(iTmp!=0){
            sess = test_sess_add_by_dnn_and_psi(test_ues3[iTmp][i], "ims", 6);
            ogs_assert(sess);
        }
        }
       
    }
    #if 1
    for (iTmp = 0; iTmp < iPthreadnum3; iTmp++)
    {
        ngaps[iTmp] =  testngap_client_n(AF_INET,iTmp);
        ogs_assert(ngaps[iTmp]);
  
            /* Send NG-Setup Reqeust */
        sendbuf = testngap_build_ng_setup_request(gnb_id+iTmp, 22);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngaps[iTmp], sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive NG-Setup Response */
       recvbuf = testgnb_ngap_read(ngaps[iTmp]);
       ABTS_PTR_NOTNULL(tc, recvbuf);
    } 
    #endif
    for (iTmp = 0; iTmp < iPthreadnum3; iTmp++)
    {
        //T_threadinfo threadInfo;
        threadInfo[iTmp].tc = tc;
        threadInfo[iTmp].clientIdx = iTmp;
		printf("threadInfo.clientIdx=%u.\r\n",threadInfo[iTmp].clientIdx);
       
        id[iTmp] = ogs_thread_create(casetests3[iTmp].func, &threadInfo[iTmp]);
        if (!id[iTmp]) return ; 

        //ogs_msleep(5000);

    }
	
    for (iTmp = 0; iTmp < iPthreadnum3; iTmp++)
    {
        ogs_thread_join_ex(id[iTmp]);
    }
        /* Two gNB disonncect from UPF */
    testgnb_gtpu_close(gtpus[0]);
    testgnb_gtpu_close(gtpus[1]);
     #if 1
    for (iTmp = 0; iTmp < iPthreadnum3; iTmp++)
    {
        testgnb_ngap_close(ngaps[iTmp]);
    }
     #endif
    for (iTmp = 0; iTmp < iPthreadnum3; iTmp++)
    {
         ogs_thread_destroy_ex(id[iTmp]); 
    }

    for (iTmp = 0; iTmp < iPthreadnum3; iTmp++)
    {
		for (i = 0; i < g_testNum; i++) {
			/********** Remove Subscriber in Database */
			ABTS_INT_EQUAL(tc, OGS_OK, test_db_remove_ue(test_ues3[iTmp][i]));
		}
	}

    /* Clear Test UE Context */
    test_ue_remove_all();
  
#endif

#if 0
    muti_ue_func(tc);
#endif
}
/* simple test 2*/
 void muti_ue_func33(void *data)
{
 
    int rv;
    ogs_socknode_t *ngap;
    ogs_socknode_t *gtpu;
    ogs_pkbuf_t *gmmbuf;
    ogs_pkbuf_t *gsmbuf;
    ogs_pkbuf_t *nasbuf;
    ogs_pkbuf_t *sendbuf;
    ogs_pkbuf_t *recvbuf;
    ogs_ngap_message_t message;
    int i;
    printf("wait for app init.\r\n");
    ogs_msleep(3000);//wait for app init
    uint8_t tmp[OGS_MAX_SDU_LEN];
    char *_gtp_payload = "34ff0024"
           "0000000100000085 010002004500001c 0c0b000040015a7a 0a2d00010a2d0002"
           "00000964cd7c291f";     
    test_ue_t **test_ue;
    test_sess_t *sess = NULL;
    test_bearer_t *qos_flow = NULL;
    af_sess_t *af_sess = NULL;
    af_npcf_policyauthorization_param_t af_param;
    bson_t *doc = NULL;
    
    T_threadinfo3 *threadInfo = (T_threadinfo3 *)data;     
    abts_case *tc = threadInfo->tc;  
    test_ue = &test_ues3[threadInfo->clientIdx];  
    test_thread_context_init(threadInfo->clientIdx);
    ngap =ngaps[threadInfo->clientIdx];
    gtpu=gtpus[0];
    for (i = 0; i < g_testNum; i++) {
    
       if (i > 0)
           test_ue[i]->ran_ue_ngap_id = test_ue[i-1]->ran_ue_ngap_id;
       else
           test_ue[i]->ran_ue_ngap_id = 0;
        sess = test_sess_find_by_psi(test_ue[i], 5);
        ogs_assert(sess);        
       
        printf("threadInfo->clientIdx[%d]:test_ue[%d]\n",threadInfo->clientIdx,i);
            /* Send Registration request */
            test_ue[i]->registration_request_param.guti = 1;
            gmmbuf = testgmm_build_registration_request(test_ue[i], NULL, false, false);
            ABTS_PTR_NOTNULL(tc, gmmbuf);
        
            test_ue[i]->registration_request_param.gmm_capability = 1;
            test_ue[i]->registration_request_param.requested_nssai = 1;
            test_ue[i]->registration_request_param.last_visited_registered_tai = 1;
            test_ue[i]->registration_request_param.ue_usage_setting = 1;
            nasbuf = testgmm_build_registration_request(test_ue[i], NULL, false, false);
            ABTS_PTR_NOTNULL(tc, nasbuf);
        
            sendbuf = testngap_build_initial_ue_message(test_ue[i], gmmbuf,
                        NGAP_RRCEstablishmentCause_mo_Signalling, false, true);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Receive Identity request */
            recvbuf = testgnb_ngap_read(ngap);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);
        
            /* Send Identity response */
            gmmbuf = testgmm_build_identity_response(test_ue[i]);
            ABTS_PTR_NOTNULL(tc, gmmbuf);
            sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Receive Authentication request */
            recvbuf = testgnb_ngap_read(ngap);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);
        
            /* Send Authentication response */
            gmmbuf = testgmm_build_authentication_response(test_ue[i]);
            ABTS_PTR_NOTNULL(tc, gmmbuf);
            sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Receive Security mode command */
            recvbuf = testgnb_ngap_read(ngap);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);
        
            /* Send Security mode complete */
            gmmbuf = testgmm_build_security_mode_complete(test_ue[i], nasbuf);
            ABTS_PTR_NOTNULL(tc, gmmbuf);
            sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Receive InitialContextSetupRequest +
             * Registration accept */
            recvbuf = testgnb_ngap_read(ngap);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);
            ABTS_INT_EQUAL(tc,
                    NGAP_ProcedureCode_id_InitialContextSetup,
                    test_ue[i]->ngap_procedure_code);
        
            /* Send UERadioCapabilityInfoIndication */
            sendbuf = testngap_build_ue_radio_capability_info_indication(test_ue[i]);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Send InitialContextSetupResponse */
            sendbuf = testngap_build_initial_context_setup_response(test_ue[i], false);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Send Registration complete */
            gmmbuf = testgmm_build_registration_complete(test_ue[i]);
            ABTS_PTR_NOTNULL(tc, gmmbuf);
            sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Receive Configuration update command */
            recvbuf = testgnb_ngap_read(ngap);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);
        
            /* Send PDU session establishment request */
            sess = test_sess_find_by_psi(test_ue[i], 5);
            //sess = test_sess_add_by_dnn_and_psi(test_ue, "internet", 5);
            ogs_assert(sess);
        
            sess->ul_nas_transport_param.request_type =
                OGS_NAS_5GS_REQUEST_TYPE_INITIAL;
            sess->ul_nas_transport_param.dnn = 1;
            sess->ul_nas_transport_param.s_nssai = 1;
        
            sess->pdu_session_establishment_param.ssc_mode = 1;
            sess->pdu_session_establishment_param.epco = 1;
        
            gsmbuf = testgsm_build_pdu_session_establishment_request(sess);
            ABTS_PTR_NOTNULL(tc, gsmbuf);
            gmmbuf = testgmm_build_ul_nas_transport(sess,
                    OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
            ABTS_PTR_NOTNULL(tc, gmmbuf);
            sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Receive PDUSessionResourceSetupRequest +
             * DL NAS transport +
             * PDU session establishment accept */
            recvbuf = testgnb_ngap_read(ngap);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);
            ABTS_INT_EQUAL(tc,
                    NGAP_ProcedureCode_id_PDUSessionResourceSetup,
                    test_ue[i]->ngap_procedure_code);
        
            /* Send GTP-U ICMP Packet */
            qos_flow = test_qos_flow_find_by_qfi(sess, 1);
            ogs_assert(qos_flow);
            rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Send PDUSessionResourceSetupResponse */
            sendbuf = testngap_sess_build_pdu_session_resource_setup_response(sess);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Receive GTP-U ICMP Packet */
            recvbuf = testgnb_gtpu_read(gtpu);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            ogs_pkbuf_free(recvbuf);
        
            /* Send GTP-U ICMP Packet */
            rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Receive GTP-U ICMP Packet */
            recvbuf = testgnb_gtpu_read(gtpu);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            ogs_pkbuf_free(recvbuf);
    #if 0   
            /* Send GTP-U Router Solicitation */
            rv = test_gtpu_send_slacc_rs(gtpu, qos_flow);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Receive GTP-U Router Advertisement */
            recvbuf = test_gtpu_read(gtpu);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testgtpu_recv(test_ue[i], recvbuf);
    #endif    
            /* Send PDU session establishment request */
            sess = test_sess_find_by_psi(test_ue[i], 6);
            //sess = test_sess_add_by_dnn_and_psi(test_ue, "ims", 6);
            ogs_assert(sess);
        
            sess->ul_nas_transport_param.request_type =
                OGS_NAS_5GS_REQUEST_TYPE_INITIAL;
            sess->ul_nas_transport_param.dnn = 1;
            sess->ul_nas_transport_param.s_nssai = 1;
        
            sess->pdu_session_establishment_param.ssc_mode = 1;
            sess->pdu_session_establishment_param.epco = 1;
        
            gsmbuf = testgsm_build_pdu_session_establishment_request(sess);
            ABTS_PTR_NOTNULL(tc, gsmbuf);
            gmmbuf = testgmm_build_ul_nas_transport(sess,
                    OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
            ABTS_PTR_NOTNULL(tc, gmmbuf);
            sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Receive PDU session establishment accept */
            recvbuf = testgnb_ngap_read(ngap);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);
        
            /* Send PDUSessionResourceSetupResponse */
            sendbuf = testngap_sess_build_pdu_session_resource_setup_response(sess);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
        
            /* Send GTP-U ICMP Packet */
            qos_flow = test_qos_flow_find_by_qfi(sess, 1);
            ogs_assert(qos_flow);
            rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Receive GTP-U ICMP Packet */
            recvbuf = testgnb_gtpu_read(gtpu);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            ogs_pkbuf_free(recvbuf);
        
            /* Send AA-Request */
            uint8_t *rx_sid = NULL;
            test_rx_send_aar_audio(&rx_sid, sess,
                    OGS_DIAM_RX_SUBSCRIPTION_ID_TYPE_END_USER_IMSI, 1, 1);
        
#if 0
            //模拟一个control类型的aar
            uint8_t *rx_sid2 = NULL;
            test_rx_send_aar_ctrl(&rx_sid2, sess,
                    OGS_DIAM_RX_SUBSCRIPTION_ID_TYPE_END_USER_SIP_URI);
        
            uint8_t *rx_sid3 = NULL;
            test_rx_send_aar_ctrl(&rx_sid3, sess,
                    OGS_DIAM_RX_SUBSCRIPTION_ID_TYPE_END_USER_SIP_URI);
#endif
#if 0
            /* Add AF-Session */
            af_sess = af_sess_add_by_ue_address(&sess->ue_ip);
            ogs_assert(af_sess);
        
            af_sess->supi = ogs_strdup(test_ue->supi);
            ogs_assert(af_sess->supi);
        
            af_sess->dnn = ogs_strdup(sess->dnn);
            ogs_assert(af_sess->dnn);
        
            af_local_discover_and_send(
                    OGS_SBI_SERVICE_TYPE_NBSF_MANAGEMENT,
                    af_sess, NULL,
                    af_nbsf_management_build_discover);
        
            /* Wait for PCF-Discovery */
            ogs_msleep(100);
        
            /* Send AF-Session : CREATE */
            memset(&af_param, 0, sizeof(af_param));
            af_param.med_type = OpenAPI_media_type_AUDIO;
            af_param.qos_type = 1;
            af_param.flow_type = 99; /* For ping test */
        
            af_local_send_to_pcf(af_sess, &af_param,
                    af_npcf_policyauthorization_build_create);
#endif
        
            /* Receive PDUSessionResourceModifyRequest +
             * DL NAS transport +
             * PDU session modification command */
            recvbuf = testgnb_ngap_read(ngap);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);
            ABTS_INT_EQUAL(tc,
                    NGAP_ProcedureCode_id_PDUSessionResourceModify,
                    test_ue[i]->ngap_procedure_code);
        
            /* Send PDU session resource modify response */
            qos_flow = test_qos_flow_find_by_qfi(sess, 2);
            ogs_assert(qos_flow);
        
            sendbuf = testngap_build_qos_flow_resource_modify_response(qos_flow);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Send PDU session resource modify complete */
            sess->ul_nas_transport_param.request_type =
                OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
            sess->ul_nas_transport_param.dnn = 0;
            sess->ul_nas_transport_param.s_nssai = 0;
        
            sess->pdu_session_establishment_param.ssc_mode = 0;
            sess->pdu_session_establishment_param.epco = 0;
        
            gsmbuf = testgsm_build_pdu_session_modification_complete(sess);
            ABTS_PTR_NOTNULL(tc, gsmbuf);
            gmmbuf = testgmm_build_ul_nas_transport(sess,
                    OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
            ABTS_PTR_NOTNULL(tc, gmmbuf);
            sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Wait for PDU session resource modify complete */
            ogs_msleep(100);
        
            //test_rx_send_aar_audio(&rx_sid, sess,
            //        OGS_DIAM_RX_SUBSCRIPTION_ID_TYPE_END_USER_IMSI, 1, 1);
                    
            /* Send GTP-U ICMP Packet */
            qos_flow = test_qos_flow_find_by_qfi(sess, 2);
            ogs_assert(qos_flow);
            rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Receive GTP-U ICMP Packet */
            recvbuf = testgnb_gtpu_read(gtpu);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            ogs_pkbuf_free(recvbuf);
        
            /* Waiting for creating dedicated QoS flow in PFCP protocol */
            ogs_msleep(100);
        
            /* Send AF-Session : DELETE */
            //af_local_send_to_pcf(af_sess, NULL,
            //        af_npcf_policyauthorization_build_delete);
            /* Send Session-Termination-Request */  
         
            test_rx_send_str(rx_sid);
        
            /* Receive PDUSessionResourceModifyRequest +
             * DL NAS transport +
             * PDU session modification command */
            recvbuf = testgnb_ngap_read(ngap);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);
            ABTS_INT_EQUAL(tc,
                    NGAP_ProcedureCode_id_PDUSessionResourceModify,
                    test_ue[i]->ngap_procedure_code);
        
            /* Send PDU session resource modify response */
            qos_flow = test_qos_flow_find_by_qfi(sess, 2);
            ogs_assert(qos_flow);
        
            sendbuf = testngap_build_qos_flow_resource_release_response(qos_flow);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Send PDU session resource modify complete */
            sess->ul_nas_transport_param.request_type =
                OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
            sess->ul_nas_transport_param.dnn = 0;
            sess->ul_nas_transport_param.s_nssai = 0;
        
            sess->pdu_session_establishment_param.ssc_mode = 0;
            sess->pdu_session_establishment_param.epco = 0;
        
            gsmbuf = testgsm_build_pdu_session_modification_complete(sess);
            ABTS_PTR_NOTNULL(tc, gsmbuf);
            gmmbuf = testgmm_build_ul_nas_transport(sess,
                    OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
            ABTS_PTR_NOTNULL(tc, gmmbuf);
            sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Wait for PDU session resource modify complete */
            ogs_msleep(100);   
        
            /* Test Bearer Remove */
            test_bearer_remove(qos_flow);
        
            //测试在5QI=5上继续发送报文
    #if 1
            /* Send GTP-U ICMP Packet */
            qos_flow = test_qos_flow_find_by_qfi(sess, 1);
            ogs_assert(qos_flow);
            rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Receive GTP-U ICMP Packet */
            recvbuf = testgnb_gtpu_read(gtpu);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            ogs_pkbuf_free(recvbuf);
    #endif
        
            ogs_msleep(100);  
        
            /* Send UEContextReleaseRequest */
            sendbuf = testngap_build_ue_context_release_request(test_ue[i],
                    NGAP_Cause_PR_radioNetwork, NGAP_CauseRadioNetwork_user_inactivity,
                    true);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Receive UEContextReleaseCommand */
            recvbuf = testgnb_ngap_read(ngap);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);
            ABTS_INT_EQUAL(tc,
                    NGAP_ProcedureCode_id_UEContextRelease,
                    test_ue[i]->ngap_procedure_code);
        
            /* Send UEContextReleaseComplete */
            sendbuf = testngap_build_ue_context_release_complete(test_ue[i]);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Send De-registration request */
            gmmbuf = testgmm_build_de_registration_request(test_ue[i], 1, true, false);
            ABTS_PTR_NOTNULL(tc, gmmbuf);
            sendbuf = testngap_build_initial_ue_message(test_ue[i], gmmbuf,
                        NGAP_RRCEstablishmentCause_mo_Signalling, true, false);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
        
            /* Receive UEContextReleaseCommand */
            recvbuf = testgnb_ngap_read(ngap);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);
            ABTS_INT_EQUAL(tc,
                    NGAP_ProcedureCode_id_UEContextRelease,
                    test_ue[i]->ngap_procedure_code);
        
            /* Send UEContextReleaseComplete */
            sendbuf = testngap_build_ue_context_release_complete(test_ue[i]);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

	

        }
    ogs_msleep(500);   

    test_context_final_ex();



}
 
 /*  simple test3*/

  void muti_ue_func35(void *data)
 { 
        int rv;
        ogs_socknode_t *ngap;
        ogs_socknode_t *gtpu;
        ogs_pkbuf_t *gmmbuf;
        ogs_pkbuf_t *gsmbuf;
        ogs_pkbuf_t *nasbuf;
        ogs_pkbuf_t *sendbuf;
        ogs_pkbuf_t *recvbuf;
        ogs_ngap_message_t message;
        int i;
        printf("wait for app init.\r\n");
        ogs_msleep(3000);//wait for app init
        uint8_t tmp[OGS_MAX_SDU_LEN];
        char *_gtp_payload = "34ff0024"
        "0000000100000085 010002004500001c 0c0b000040015a7a 0a2d00010a2d0002"
        "00000964cd7c291f";     
        test_ue_t **test_ue;
        test_sess_t *sess = NULL;
        test_bearer_t *qos_flow = NULL;
        af_sess_t *af_sess = NULL;
        af_npcf_policyauthorization_param_t af_param;
        bson_t *doc = NULL;

        T_threadinfo3 *threadInfo = (T_threadinfo3 *)data;     
        abts_case *tc = threadInfo->tc;  
        test_ue = &test_ues3[threadInfo->clientIdx];  
        test_thread_context_init(threadInfo->clientIdx);
        ngap =ngaps[threadInfo->clientIdx];
        gtpu=gtpus[0];
        for (i = 0; i < g_testNum; i++) {

        if (i > 0)
        test_ue[i]->ran_ue_ngap_id = test_ue[i-1]->ran_ue_ngap_id;
        else
        test_ue[i]->ran_ue_ngap_id = 0;
        sess = test_sess_find_by_psi(test_ue[i], 5);
        ogs_assert(sess);        

        printf("threadInfo->clientIdx[%d]:test_ue[%d]\n",threadInfo->clientIdx,i);
        /* Send Registration request */
        test_ue[i]->registration_request_param.guti = 1;
        gmmbuf = testgmm_build_registration_request(test_ue[i], NULL, false, false);
        ABTS_PTR_NOTNULL(tc, gmmbuf);
    
        test_ue[i]->registration_request_param.gmm_capability = 1;
        test_ue[i]->registration_request_param.requested_nssai = 1;
        test_ue[i]->registration_request_param.last_visited_registered_tai = 1;
        test_ue[i]->registration_request_param.ue_usage_setting = 1;
        nasbuf = testgmm_build_registration_request(test_ue[i], NULL, false, false);
        ABTS_PTR_NOTNULL(tc, nasbuf);
    
        sendbuf = testngap_build_initial_ue_message(test_ue[i], gmmbuf,
                    NGAP_RRCEstablishmentCause_mo_Signalling, false, true);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
        /* Receive Identity request */
        recvbuf = testgnb_ngap_read(ngap);
        ABTS_PTR_NOTNULL(tc, recvbuf);
        testngap_recv(test_ue[i], recvbuf);
    
        /* Send Identity response */
        gmmbuf = testgmm_build_identity_response(test_ue[i]);
        ABTS_PTR_NOTNULL(tc, gmmbuf);
        sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
        /* Receive Authentication request */
        recvbuf = testgnb_ngap_read(ngap);
        ABTS_PTR_NOTNULL(tc, recvbuf);
        testngap_recv(test_ue[i], recvbuf);
    
        /* Send Authentication response */
        gmmbuf = testgmm_build_authentication_response(test_ue[i]);
        ABTS_PTR_NOTNULL(tc, gmmbuf);
        sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
        /* Receive Security mode command */
        recvbuf = testgnb_ngap_read(ngap);
        ABTS_PTR_NOTNULL(tc, recvbuf);
        testngap_recv(test_ue[i], recvbuf);
    
        /* Send Security mode complete */
        gmmbuf = testgmm_build_security_mode_complete(test_ue[i], nasbuf);
        ABTS_PTR_NOTNULL(tc, gmmbuf);
        sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
        /* Receive InitialContextSetupRequest +
         * Registration accept */
        recvbuf = testgnb_ngap_read(ngap);
        ABTS_PTR_NOTNULL(tc, recvbuf);
        testngap_recv(test_ue[i], recvbuf);
        ABTS_INT_EQUAL(tc,
                NGAP_ProcedureCode_id_InitialContextSetup,
                test_ue[i]->ngap_procedure_code);
    
        /* Send UERadioCapabilityInfoIndication */
        sendbuf = testngap_build_ue_radio_capability_info_indication(test_ue[i]);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
        /* Send InitialContextSetupResponse */
        sendbuf = testngap_build_initial_context_setup_response(test_ue[i], false);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
        /* Send Registration complete */
        gmmbuf = testgmm_build_registration_complete(test_ue[i]);
        ABTS_PTR_NOTNULL(tc, gmmbuf);
        sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
        /* Receive Configuration update command */
        recvbuf = testgnb_ngap_read(ngap);
        ABTS_PTR_NOTNULL(tc, recvbuf);
        testngap_recv(test_ue[i], recvbuf);
    
        /* Send PDU session establishment request */
        sess = test_sess_find_by_psi(test_ue[i], 5);
        //sess = test_sess_add_by_dnn_and_psi(test_ue, "internet", 5);
        ogs_assert(sess);
    
        sess->ul_nas_transport_param.request_type =
            OGS_NAS_5GS_REQUEST_TYPE_INITIAL;
        sess->ul_nas_transport_param.dnn = 1;
        sess->ul_nas_transport_param.s_nssai = 1;
    
        sess->pdu_session_establishment_param.ssc_mode = 1;
        sess->pdu_session_establishment_param.epco = 1;
    
        gsmbuf = testgsm_build_pdu_session_establishment_request(sess);
        ABTS_PTR_NOTNULL(tc, gsmbuf);
        gmmbuf = testgmm_build_ul_nas_transport(sess,
                OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
        ABTS_PTR_NOTNULL(tc, gmmbuf);
        sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
        /* Receive PDUSessionResourceSetupRequest +
         * DL NAS transport +
         * PDU session establishment accept */
        recvbuf = testgnb_ngap_read(ngap);
        ABTS_PTR_NOTNULL(tc, recvbuf);
        testngap_recv(test_ue[i], recvbuf);
        ABTS_INT_EQUAL(tc,
                NGAP_ProcedureCode_id_PDUSessionResourceSetup,
                test_ue[i]->ngap_procedure_code);
    
        /* Send GTP-U ICMP Packet */
        qos_flow = test_qos_flow_find_by_qfi(sess, 1);
        ogs_assert(qos_flow);
        rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
        /* Send PDUSessionResourceSetupResponse */
        sendbuf = testngap_sess_build_pdu_session_resource_setup_response(sess);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
        /* Receive GTP-U ICMP Packet */
        recvbuf = testgnb_gtpu_read(gtpu);
        ABTS_PTR_NOTNULL(tc, recvbuf);
        ogs_pkbuf_free(recvbuf);
    
        /* Send GTP-U ICMP Packet */
        rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
        /* Receive GTP-U ICMP Packet */
        recvbuf = testgnb_gtpu_read(gtpu);
        ABTS_PTR_NOTNULL(tc, recvbuf);
        ogs_pkbuf_free(recvbuf);
    #if 0
        /* Send GTP-U Router Solicitation */
        rv = test_gtpu_send_slacc_rs(gtpu, qos_flow);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
        /* Receive GTP-U Router Advertisement */
        recvbuf = test_gtpu_read(gtpu);
        ABTS_PTR_NOTNULL(tc, recvbuf);
        testgtpu_recv(test_ue[i], recvbuf);
    #endif
        /* Send PDU session establishment request */
        sess = test_sess_find_by_psi(test_ue[i], 6);
        //sess = test_sess_add_by_dnn_and_psi(test_ue, "ims", 6);
        ogs_assert(sess);
    
        sess->ul_nas_transport_param.request_type =
            OGS_NAS_5GS_REQUEST_TYPE_INITIAL;
        sess->ul_nas_transport_param.dnn = 1;
        sess->ul_nas_transport_param.s_nssai = 1;
    
        sess->pdu_session_establishment_param.ssc_mode = 1;
        sess->pdu_session_establishment_param.epco = 1;
    
        gsmbuf = testgsm_build_pdu_session_establishment_request(sess);
        ABTS_PTR_NOTNULL(tc, gsmbuf);
        gmmbuf = testgmm_build_ul_nas_transport(sess,
                OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
        ABTS_PTR_NOTNULL(tc, gmmbuf);
        sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
        /* Receive PDU session establishment accept */
        recvbuf = testgnb_ngap_read(ngap);
        ABTS_PTR_NOTNULL(tc, recvbuf);
        testngap_recv(test_ue[i], recvbuf);
    
        /* Send PDUSessionResourceSetupResponse */
        sendbuf = testngap_sess_build_pdu_session_resource_setup_response(sess);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
    
        /* Send AA-Request */
        uint8_t *rx_sid = NULL;
        test_rx_send_aar_audio(&rx_sid, sess,
                OGS_DIAM_RX_SUBSCRIPTION_ID_TYPE_END_USER_IMSI, 1, 1);
#if 0
        /* Add AF-Session */
        af_sess = af_sess_add_by_ue_address(&sess->ue_ip);
        ogs_assert(af_sess);
    
        af_sess->supi = ogs_strdup(test_ue->supi);
        ogs_assert(af_sess->supi);
    
        af_sess->dnn = ogs_strdup(sess->dnn);
        ogs_assert(af_sess->dnn);
    
        af_local_discover_and_send(
                OGS_SBI_SERVICE_TYPE_NBSF_MANAGEMENT,
                af_sess, NULL,
                af_nbsf_management_build_discover);
    
        /* Wait for PCF-Discovery */
        ogs_msleep(100);
    
        /* Send AF-Session : CREATE */
        memset(&af_param, 0, sizeof(af_param));
        af_param.med_type = OpenAPI_media_type_AUDIO;
        af_param.qos_type = 1;
        af_param.flow_type = 99; /* For ping test */
    
        af_local_send_to_pcf(af_sess, &af_param,
                af_npcf_policyauthorization_build_create);
#endif
        ogs_msleep(100);
        /* Receive PDUSessionResourceModifyRequest +
         * DL NAS transport +
         * PDU session modification command */
        recvbuf = testgnb_ngap_read(ngap);
        ABTS_PTR_NOTNULL(tc, recvbuf);
        testngap_recv(test_ue[i], recvbuf);
        ABTS_INT_EQUAL(tc,
                NGAP_ProcedureCode_id_PDUSessionResourceModify,
                test_ue[i]->ngap_procedure_code);
    
        /* Send PDU session resource modify response */
        qos_flow = test_qos_flow_find_by_qfi(sess, 2);
        ogs_assert(qos_flow);
    
        sendbuf = testngap_build_qos_flow_resource_modify_response(qos_flow);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
        /* Send PDU session resource modify complete */
        sess->ul_nas_transport_param.request_type =
            OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
        sess->ul_nas_transport_param.dnn = 0;
        sess->ul_nas_transport_param.s_nssai = 0;
    
        sess->pdu_session_establishment_param.ssc_mode = 0;
        sess->pdu_session_establishment_param.epco = 0;
    
        gsmbuf = testgsm_build_pdu_session_modification_complete(sess);
        ABTS_PTR_NOTNULL(tc, gsmbuf);
        gmmbuf = testgmm_build_ul_nas_transport(sess,
                OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
        ABTS_PTR_NOTNULL(tc, gmmbuf);
        sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
        /* Wait for PDU session resource modify complete */
        ogs_msleep(100);
    
        /* Send GTP-U ICMP Packet */
        qos_flow = test_qos_flow_find_by_qfi(sess, 2);
        ogs_assert(qos_flow);
        rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
        /* Receive GTP-U ICMP Packet */
        recvbuf = testgnb_gtpu_read(gtpu);
        ABTS_PTR_NOTNULL(tc, recvbuf);
        ogs_pkbuf_free(recvbuf);
    
        //模拟一个control类型的aar
        uint8_t *rx_sid2 = NULL;
        test_rx_send_aar_ctrl(&rx_sid2, sess,
                OGS_DIAM_RX_SUBSCRIPTION_ID_TYPE_END_USER_SIP_URI);
    
        uint8_t *rx_sid3 = NULL;
        test_rx_send_aar_ctrl(&rx_sid3, sess,
                OGS_DIAM_RX_SUBSCRIPTION_ID_TYPE_END_USER_SIP_URI);
        /* Waiting for creating dedicated QoS flow in PFCP protocol */
        ogs_msleep(100);
    
    
        /* Test Bearer Remove */
        test_bearer_remove(qos_flow);
    
        /* Send UEContextReleaseRequest */
        sendbuf = testngap_build_ue_context_release_request(test_ue[i],
                NGAP_Cause_PR_radioNetwork, NGAP_CauseRadioNetwork_user_inactivity,
                true);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
        /* Receive UEContextReleaseCommand */
        recvbuf = testgnb_ngap_read(ngap);
        ABTS_PTR_NOTNULL(tc, recvbuf);
        testngap_recv(test_ue[i], recvbuf);
        ABTS_INT_EQUAL(tc,
                NGAP_ProcedureCode_id_UEContextRelease,
                test_ue[i]->ngap_procedure_code);
    
        /* Send UEContextReleaseComplete */
        sendbuf = testngap_build_ue_context_release_complete(test_ue[i]);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
        /* Send De-registration request */
        gmmbuf = testgmm_build_de_registration_request(test_ue[i], 1, true, false);
        ABTS_PTR_NOTNULL(tc, gmmbuf);
        sendbuf = testngap_build_initial_ue_message(test_ue[i], gmmbuf,
                    NGAP_RRCEstablishmentCause_mo_Signalling, true, false);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
        /* Receive UEContextReleaseCommand */
        recvbuf = testgnb_ngap_read(ngap);
        ABTS_PTR_NOTNULL(tc, recvbuf);
        testngap_recv(test_ue[i], recvbuf);
        ABTS_INT_EQUAL(tc,
                NGAP_ProcedureCode_id_UEContextRelease,
                test_ue[i]->ngap_procedure_code);
    
        /* Send UEContextReleaseComplete */
        sendbuf = testngap_build_ue_context_release_complete(test_ue[i]);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
        ogs_msleep(300);

    }
    test_context_final_ex();


 }
 /*  simple test1*/
 void muti_ue_func32(void *data)
  {
      int rv;
      ogs_socknode_t *ngap;
      ogs_socknode_t *gtpu;
      ogs_pkbuf_t *gmmbuf;
      ogs_pkbuf_t *gsmbuf;
      ogs_pkbuf_t *nasbuf;
      ogs_pkbuf_t *sendbuf;
      ogs_pkbuf_t *recvbuf;
      ogs_ngap_message_t message;
      int i;
      printf("wait for app init.\r\n");
      ogs_msleep(3000);//wait for app init
      uint8_t tmp[OGS_MAX_SDU_LEN];
      char *_gtp_payload = "34ff0024"
             "0000000100000085 010002004500001c 0c0b000040015a7a 0a2d00010a2d0002"
             "00000964cd7c291f";     
      test_ue_t **test_ue;
      test_sess_t *sess = NULL;
      test_bearer_t *qos_flow = NULL;
      af_sess_t *af_sess = NULL;
      af_npcf_policyauthorization_param_t af_param;
      bson_t *doc = NULL;
  
      T_threadinfo3 *threadInfo = (T_threadinfo3 *)data;     
      abts_case *tc = threadInfo->tc;  
      test_ue = &test_ues3[threadInfo->clientIdx];  
      test_thread_context_init(threadInfo->clientIdx);
      ngap =ngaps[threadInfo->clientIdx];
      gtpu=gtpus[0];
      for (i = 0; i < g_testNum; i++) {
 
         if (i > 0)
             test_ue[i]->ran_ue_ngap_id = test_ue[i-1]->ran_ue_ngap_id;
         else
             test_ue[i]->ran_ue_ngap_id = 0;
          sess = test_sess_find_by_psi(test_ue[i], 5);
          ogs_assert(sess);        
         
          printf("threadInfo->clientIdx[%d]:test_ue[%d]\n",threadInfo->clientIdx,i);

          
          
          /* Send Registration request */
          test_ue[i]->registration_request_param.guti = 1;
          gmmbuf = testgmm_build_registration_request(test_ue[i], NULL, false, false);
          ABTS_PTR_NOTNULL(tc, gmmbuf);
          
          test_ue[i]->registration_request_param.gmm_capability = 1;
          test_ue[i]->registration_request_param.requested_nssai = 1;
          test_ue[i]->registration_request_param.last_visited_registered_tai = 1;
          test_ue[i]->registration_request_param.ue_usage_setting = 1;
          //test_ue[i]->nr_tai.tac.v=i%2+1;
          nasbuf = testgmm_build_registration_request(test_ue[i], NULL, false, false);
          ABTS_PTR_NOTNULL(tc, nasbuf);
          
          sendbuf = testngap_build_initial_ue_message(test_ue[i], gmmbuf,
                      NGAP_RRCEstablishmentCause_mo_Signalling, false, true);
          ABTS_PTR_NOTNULL(tc, sendbuf);
          rv = testgnb_ngap_send(ngap, sendbuf);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Receive Identity request */
          recvbuf = testgnb_ngap_read(ngap);
          ABTS_PTR_NOTNULL(tc, recvbuf);
          testngap_recv(test_ue[i], recvbuf);
          
          /* Send Identity response */
          gmmbuf = testgmm_build_identity_response(test_ue[i]);
          ABTS_PTR_NOTNULL(tc, gmmbuf);
          sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
          ABTS_PTR_NOTNULL(tc, sendbuf);
          rv = testgnb_ngap_send(ngap, sendbuf);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Receive Authentication request */
          recvbuf = testgnb_ngap_read(ngap);
          ABTS_PTR_NOTNULL(tc, recvbuf);
          testngap_recv(test_ue[i], recvbuf);
          
          /* Send Authentication response */
          gmmbuf = testgmm_build_authentication_response(test_ue[i]);
          ABTS_PTR_NOTNULL(tc, gmmbuf);
          sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
          ABTS_PTR_NOTNULL(tc, sendbuf);
          rv = testgnb_ngap_send(ngap, sendbuf);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Receive Security mode command */
          recvbuf = testgnb_ngap_read(ngap);
          ABTS_PTR_NOTNULL(tc, recvbuf);
          testngap_recv(test_ue[i], recvbuf);
          
          /* Send Security mode complete */
          gmmbuf = testgmm_build_security_mode_complete(test_ue[i], nasbuf);
          ABTS_PTR_NOTNULL(tc, gmmbuf);
          sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
          ABTS_PTR_NOTNULL(tc, sendbuf);
          rv = testgnb_ngap_send(ngap, sendbuf);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Receive InitialContextSetupRequest +
           * Registration accept */
          recvbuf = testgnb_ngap_read(ngap);
          ABTS_PTR_NOTNULL(tc, recvbuf);
          testngap_recv(test_ue[i], recvbuf);
          ABTS_INT_EQUAL(tc,
                  NGAP_ProcedureCode_id_InitialContextSetup,
                  test_ue[i]->ngap_procedure_code);
          
          /* Send UERadioCapabilityInfoIndication */
          sendbuf = testngap_build_ue_radio_capability_info_indication(test_ue[i]);
          ABTS_PTR_NOTNULL(tc, sendbuf);
          rv = testgnb_ngap_send(ngap, sendbuf);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Send InitialContextSetupResponse */
          sendbuf = testngap_build_initial_context_setup_response(test_ue[i], false);
          ABTS_PTR_NOTNULL(tc, sendbuf);
          rv = testgnb_ngap_send(ngap, sendbuf);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Send Registration complete */
          gmmbuf = testgmm_build_registration_complete(test_ue[i]);
          ABTS_PTR_NOTNULL(tc, gmmbuf);
          sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
          ABTS_PTR_NOTNULL(tc, sendbuf);
          rv = testgnb_ngap_send(ngap, sendbuf);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Receive Configuration update command */
          recvbuf = testgnb_ngap_read(ngap);
          ABTS_PTR_NOTNULL(tc, recvbuf);
          testngap_recv(test_ue[i], recvbuf);
          
          /* Send PDU session establishment request */
          sess = test_sess_find_by_psi(test_ue[i], 5);
          //sess = test_sess_add_by_dnn_and_psi(test_ue[i], "internet", 5);
          ogs_assert(sess);
          
          sess->ul_nas_transport_param.request_type =
              OGS_NAS_5GS_REQUEST_TYPE_INITIAL;
          sess->ul_nas_transport_param.dnn = 1;
          sess->ul_nas_transport_param.s_nssai = 1;
          
          sess->pdu_session_establishment_param.ssc_mode = 1;
          sess->pdu_session_establishment_param.epco = 1;
          
          gsmbuf = testgsm_build_pdu_session_establishment_request(sess);
          ABTS_PTR_NOTNULL(tc, gsmbuf);
          gmmbuf = testgmm_build_ul_nas_transport(sess,
                  OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
          ABTS_PTR_NOTNULL(tc, gmmbuf);
          sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
          ABTS_PTR_NOTNULL(tc, sendbuf);
          rv = testgnb_ngap_send(ngap, sendbuf);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Receive PDUSessionResourceSetupRequest +
           * DL NAS transport +
           * PDU session establishment accept */
          recvbuf = testgnb_ngap_read(ngap);
          ABTS_PTR_NOTNULL(tc, recvbuf);
          testngap_recv(test_ue[i], recvbuf);
          ABTS_INT_EQUAL(tc,
                  NGAP_ProcedureCode_id_PDUSessionResourceSetup,
                  test_ue[i]->ngap_procedure_code);
          
          /* Send GTP-U ICMP Packet */
          qos_flow = test_qos_flow_find_by_qfi(sess, 1);
          ogs_assert(qos_flow);
          rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          //printf("1111111\n");
          /* Send PDUSessionResourceSetupResponse */
          sendbuf = testngap_sess_build_pdu_session_resource_setup_response(sess);
          ABTS_PTR_NOTNULL(tc, sendbuf);
          rv = testgnb_ngap_send(ngap, sendbuf);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Receive GTP-U ICMP Packet */
          recvbuf = testgnb_gtpu_read(gtpu);
          ABTS_PTR_NOTNULL(tc, recvbuf);
          ogs_pkbuf_free(recvbuf);
          
          /* Send GTP-U ICMP Packet */
          rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Receive GTP-U ICMP Packet */
          recvbuf = testgnb_gtpu_read(gtpu);
          ABTS_PTR_NOTNULL(tc, recvbuf);
          ogs_pkbuf_free(recvbuf);
    #if 0       
          /* Send GTP-U Router Solicitation */
          rv = test_gtpu_send_slacc_rs(gtpu, qos_flow);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Receive GTP-U Router Advertisement */
          recvbuf = test_gtpu_read(gtpu);
          ABTS_PTR_NOTNULL(tc, recvbuf);
          testgtpu_recv(test_ue[i], recvbuf);
     #endif     
          /* Send PDU session establishment request */
          //sess = test_sess_add_by_dnn_and_psi(test_ue[i], "ims", 6);
          sess = test_sess_find_by_psi(test_ue[i], 6);
          ogs_assert(sess);
          
          sess->ul_nas_transport_param.request_type =
              OGS_NAS_5GS_REQUEST_TYPE_INITIAL;
          sess->ul_nas_transport_param.dnn = 1;
          sess->ul_nas_transport_param.s_nssai = 1;
          
          sess->pdu_session_establishment_param.ssc_mode = 1;
          sess->pdu_session_establishment_param.epco = 1;
          
          gsmbuf = testgsm_build_pdu_session_establishment_request(sess);
          ABTS_PTR_NOTNULL(tc, gsmbuf);
          gmmbuf = testgmm_build_ul_nas_transport(sess,
                  OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
          ABTS_PTR_NOTNULL(tc, gmmbuf);
          sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
          ABTS_PTR_NOTNULL(tc, sendbuf);
          rv = testgnb_ngap_send(ngap, sendbuf);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Receive PDU session establishment accept */
          recvbuf = testgnb_ngap_read(ngap);
          ABTS_PTR_NOTNULL(tc, recvbuf);
          testngap_recv(test_ue[i], recvbuf);
          //printf("222222222222222\n");
          /* Send PDUSessionResourceSetupResponse */
          sendbuf = testngap_sess_build_pdu_session_resource_setup_response(sess);
          ABTS_PTR_NOTNULL(tc, sendbuf);
          rv = testgnb_ngap_send(ngap, sendbuf);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Add AF-Session */
          af_sess = af_sess_add_by_ue_address(&sess->ue_ip);
          ogs_assert(af_sess);
          
          af_sess->supi = ogs_strdup(test_ue[i]->supi);
          ogs_assert(af_sess->supi);
          
          af_sess->dnn = ogs_strdup(sess->dnn);
          ogs_assert(af_sess->dnn);
          
          af_local_discover_and_send(
                  OGS_SBI_SERVICE_TYPE_NBSF_MANAGEMENT,
                  af_sess, NULL,
                  af_nbsf_management_build_discover);
          
          /* Wait for PCF-Discovery */
          ogs_msleep(100);
          
          /* Send AF-Session : CREATE */
          memset(&af_param, 0, sizeof(af_param));
          af_param.med_type = OpenAPI_media_type_AUDIO;
          af_param.qos_type = 1;
          af_param.flow_type = 99; /* For ping test */
          
          af_local_send_to_pcf(af_sess, &af_param,
                  af_npcf_policyauthorization_build_create);
          
          /* Receive PDUSessionResourceModifyRequest +
           * DL NAS transport +
           * PDU session modification command */
          recvbuf = testgnb_ngap_read(ngap);
          ABTS_PTR_NOTNULL(tc, recvbuf);
          testngap_recv(test_ue[i], recvbuf);
          ABTS_INT_EQUAL(tc,
                  NGAP_ProcedureCode_id_PDUSessionResourceModify,
                  test_ue[i]->ngap_procedure_code);
          
          /* Send PDU session resource modify response */
          qos_flow = test_qos_flow_find_by_qfi(sess, 2);
          ogs_assert(qos_flow);
          
          sendbuf = testngap_build_qos_flow_resource_modify_response(qos_flow);
          ABTS_PTR_NOTNULL(tc, sendbuf);
          rv = testgnb_ngap_send(ngap, sendbuf);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Send PDU session resource modify complete */
          sess->ul_nas_transport_param.request_type =
              OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
          sess->ul_nas_transport_param.dnn = 0;
          sess->ul_nas_transport_param.s_nssai = 0;
          
          sess->pdu_session_establishment_param.ssc_mode = 0;
          sess->pdu_session_establishment_param.epco = 0;
          
          gsmbuf = testgsm_build_pdu_session_modification_complete(sess);
          ABTS_PTR_NOTNULL(tc, gsmbuf);
          gmmbuf = testgmm_build_ul_nas_transport(sess,
                  OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
          ABTS_PTR_NOTNULL(tc, gmmbuf);
          sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
          ABTS_PTR_NOTNULL(tc, sendbuf);
          rv = testgnb_ngap_send(ngap, sendbuf);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Wait for PDU session resource modify complete */
          ogs_msleep(100);
          
          /* Send GTP-U ICMP Packet */
          qos_flow = test_qos_flow_find_by_qfi(sess, 2);
          ogs_assert(qos_flow);
          rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Receive GTP-U ICMP Packet */
          recvbuf = testgnb_gtpu_read(gtpu);
          ABTS_PTR_NOTNULL(tc, recvbuf);
          ogs_pkbuf_free(recvbuf);
          
          /* Waiting for creating dedicated QoS flow in PFCP protocol */
          ogs_msleep(100);
          
          /* Send AF-Session : DELETE */
          af_local_send_to_pcf(af_sess, NULL,
                  af_npcf_policyauthorization_build_delete);
          
          /* Receive PDUSessionResourceModifyRequest +
           * DL NAS transport +
           * PDU session modification command */
          recvbuf = testgnb_ngap_read(ngap);
          ABTS_PTR_NOTNULL(tc, recvbuf);
          testngap_recv(test_ue[i], recvbuf);
          ABTS_INT_EQUAL(tc,
                  NGAP_ProcedureCode_id_PDUSessionResourceModify,
                  test_ue[i]->ngap_procedure_code);
          
          /* Send PDU session resource modify response */
          qos_flow = test_qos_flow_find_by_qfi(sess, 2);
          ogs_assert(qos_flow);
          
          sendbuf = testngap_build_qos_flow_resource_release_response(qos_flow);
          ABTS_PTR_NOTNULL(tc, sendbuf);
          rv = testgnb_ngap_send(ngap, sendbuf);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Send PDU session resource modify complete */
          sess->ul_nas_transport_param.request_type =
              OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
          sess->ul_nas_transport_param.dnn = 0;
          sess->ul_nas_transport_param.s_nssai = 0;
          
          sess->pdu_session_establishment_param.ssc_mode = 0;
          sess->pdu_session_establishment_param.epco = 0;
          
          gsmbuf = testgsm_build_pdu_session_modification_complete(sess);
          ABTS_PTR_NOTNULL(tc, gsmbuf);
          gmmbuf = testgmm_build_ul_nas_transport(sess,
                  OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
          ABTS_PTR_NOTNULL(tc, gmmbuf);
          sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
          ABTS_PTR_NOTNULL(tc, sendbuf);
          rv = testgnb_ngap_send(ngap, sendbuf);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Wait for PDU session resource modify complete */
          ogs_msleep(100);
          
          /* Test Bearer Remove */
          test_bearer_remove(qos_flow);
          
          /* Send UEContextReleaseRequest */
          sendbuf = testngap_build_ue_context_release_request(test_ue[i],
                  NGAP_Cause_PR_radioNetwork, NGAP_CauseRadioNetwork_user_inactivity,
                  true);
          ABTS_PTR_NOTNULL(tc, sendbuf);
          rv = testgnb_ngap_send(ngap, sendbuf);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Receive UEContextReleaseCommand */
          recvbuf = testgnb_ngap_read(ngap);
          ABTS_PTR_NOTNULL(tc, recvbuf);
          testngap_recv(test_ue[i], recvbuf);
          ABTS_INT_EQUAL(tc,
                  NGAP_ProcedureCode_id_UEContextRelease,
                  test_ue[i]->ngap_procedure_code);
          
          /* Send UEContextReleaseComplete */
          sendbuf = testngap_build_ue_context_release_complete(test_ue[i]);
          ABTS_PTR_NOTNULL(tc, sendbuf);
          rv = testgnb_ngap_send(ngap, sendbuf);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Send De-registration request */
          gmmbuf = testgmm_build_de_registration_request(test_ue[i], 1, true, false);
          ABTS_PTR_NOTNULL(tc, gmmbuf);
          sendbuf = testngap_build_initial_ue_message(test_ue[i], gmmbuf,
                      NGAP_RRCEstablishmentCause_mo_Signalling, true, false);
          ABTS_PTR_NOTNULL(tc, sendbuf);
          rv = testgnb_ngap_send(ngap, sendbuf);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          /* Receive UEContextReleaseCommand */
          recvbuf = testgnb_ngap_read(ngap);
          ABTS_PTR_NOTNULL(tc, recvbuf);
          testngap_recv(test_ue[i], recvbuf);
          ABTS_INT_EQUAL(tc,
                  NGAP_ProcedureCode_id_UEContextRelease,
                  test_ue[i]->ngap_procedure_code);
          
          /* Send UEContextReleaseComplete */
          sendbuf = testngap_build_ue_context_release_complete(test_ue[i]);
          ABTS_PTR_NOTNULL(tc, sendbuf);
          rv = testgnb_ngap_send(ngap, sendbuf);
          ABTS_INT_EQUAL(tc, OGS_OK, rv);
          
          ogs_msleep(300);
        
        }
      ogs_msleep(100);
      test_context_final_ex();
    }  
 /* session test1*/
 void muti_ue_func34(void *data)
 {  
 
 int rv;
 ogs_socknode_t *ngap;
 ogs_socknode_t *gtpu;
 ogs_pkbuf_t *gmmbuf;
 ogs_pkbuf_t *gsmbuf;
 ogs_pkbuf_t *nasbuf;
 ogs_pkbuf_t *sendbuf;
 ogs_pkbuf_t *recvbuf;
 ogs_ngap_message_t message;
 int i;
 printf("wait for app init.\r\n");
 ogs_msleep(3000);//wait for app init
 uint8_t tmp[OGS_MAX_SDU_LEN];
 char *_gtp_payload = "34ff0024"
        "0000000100000085 010002004500001c 0c0b000040015a7a 0a2d00010a2d0002"
        "00000964cd7c291f";     
 test_ue_t **test_ue;
 test_sess_t *sess = NULL;
 test_bearer_t *qos_flow = NULL;
 af_sess_t *af_sess = NULL;
 af_npcf_policyauthorization_param_t af_param;
 bson_t *doc = NULL;
 
 T_threadinfo3 *threadInfo = (T_threadinfo3 *)data;     
 abts_case *tc = threadInfo->tc;  
 test_ue = &test_ues3[threadInfo->clientIdx];  
 test_thread_context_init(threadInfo->clientIdx);
 ngap =ngaps[threadInfo->clientIdx];
 gtpu=gtpus[0];
 for (i = 0; i < g_testNum; i++) {
 
    if (i > 0)
        test_ue[i]->ran_ue_ngap_id = test_ue[i-1]->ran_ue_ngap_id;
    else
        test_ue[i]->ran_ue_ngap_id = 0;
     sess = test_sess_find_by_psi(test_ue[i], 5);
     ogs_assert(sess);        
    
     printf("threadInfo->clientIdx[%d]:test_ue[%d]\n",threadInfo->clientIdx,i);
         /* Send Registration request */
    test_ue[i]->registration_request_param.guti = 1;
    gmmbuf = testgmm_build_registration_request(test_ue[i], NULL, false, false);
    ABTS_PTR_NOTNULL(tc, gmmbuf);

    test_ue[i]->registration_request_param.gmm_capability = 1;
    test_ue[i]->registration_request_param.requested_nssai = 1;
    test_ue[i]->registration_request_param.last_visited_registered_tai = 1;
    test_ue[i]->registration_request_param.ue_usage_setting = 1;
    nasbuf = testgmm_build_registration_request(test_ue[i], NULL, false, false);
    ABTS_PTR_NOTNULL(tc, nasbuf);

    sendbuf = testngap_build_initial_ue_message(test_ue[i], gmmbuf,
                NGAP_RRCEstablishmentCause_mo_Signalling, false, true);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive Identity request */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);

    /* Send Identity response */
    gmmbuf = testgmm_build_identity_response(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive Authentication request */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);

    /* Send Authentication response */
    gmmbuf = testgmm_build_authentication_response(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive Security mode command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);

    /* Send Security mode complete */
    gmmbuf = testgmm_build_security_mode_complete(test_ue[i], nasbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive InitialContextSetupRequest +
     * Registration accept */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_InitialContextSetup,
            test_ue[i]->ngap_procedure_code);

    /* Send UERadioCapabilityInfoIndication */
    sendbuf = testngap_build_ue_radio_capability_info_indication(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send InitialContextSetupResponse */
    sendbuf = testngap_build_initial_context_setup_response(test_ue[i], false);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send Registration complete */
    gmmbuf = testgmm_build_registration_complete(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive Configuration update command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);

    /* Send PDU session establishment request */
    //sess = test_sess_add_by_dnn_and_psi(test_ue, "internet", 5);
    sess = test_sess_find_by_psi(test_ue[i], 5);
    ogs_assert(sess);

    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_INITIAL;
    sess->ul_nas_transport_param.dnn = 1;
    sess->ul_nas_transport_param.s_nssai = 1;

    sess->pdu_session_establishment_param.ssc_mode = 1;
    sess->pdu_session_establishment_param.epco = 1;

    gsmbuf = testgsm_build_pdu_session_establishment_request(sess);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive PDUSessionResourceSetupRequest +
     * DL NAS transport +
     * PDU session establishment accept */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_PDUSessionResourceSetup,
            test_ue[i]->ngap_procedure_code);

    /* Send GTP-U ICMP Packet */
    qos_flow = test_qos_flow_find_by_qfi(sess, 1);
    ogs_assert(qos_flow);
    rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send PDUSessionResourceSetupResponse */
    sendbuf = testngap_sess_build_pdu_session_resource_setup_response(sess);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive GTP-U ICMP Packet */
    recvbuf = testgnb_gtpu_read(gtpu);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    ogs_pkbuf_free(recvbuf);

    /* Send GTP-U ICMP Packet */
    rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive GTP-U ICMP Packet */
    recvbuf = testgnb_gtpu_read(gtpu);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    ogs_pkbuf_free(recvbuf);

    /* Send PDU session establishment request */
    //sess = test_sess_add_by_dnn_and_psi(test_ue, "ims", 6);
    sess = test_sess_find_by_psi(test_ue[i], 6);
    ogs_assert(sess);

    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_INITIAL;
    sess->ul_nas_transport_param.dnn = 1;
    sess->ul_nas_transport_param.s_nssai = 1;

    sess->pdu_session_establishment_param.ssc_mode = 1;
    sess->pdu_session_establishment_param.epco = 1;

    gsmbuf = testgsm_build_pdu_session_establishment_request(sess);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive PDU session establishment accept */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);

    /* Send PDUSessionResourceSetupResponse */
    sendbuf = testngap_sess_build_pdu_session_resource_setup_response(sess);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive PDUSessionResourceModifyRequest +
     * DL NAS transport +
     * PDU session modification command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_PDUSessionResourceModify,
            test_ue[i]->ngap_procedure_code);

    /* Send PDU session resource modify response */
    qos_flow = test_qos_flow_find_by_qfi(sess, 2);
    ogs_assert(qos_flow);

    sendbuf = testngap_build_qos_flow_resource_modify_response(qos_flow);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send PDU session resource modify complete */
    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
    sess->ul_nas_transport_param.dnn = 0;
    sess->ul_nas_transport_param.s_nssai = 0;

    sess->pdu_session_establishment_param.ssc_mode = 0;
    sess->pdu_session_establishment_param.epco = 0;

    gsmbuf = testgsm_build_pdu_session_modification_complete(sess);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Waiting for creating dedicated QoS flow in PFCP protocol */
    ogs_msleep(100);

    /* Send GTP-U ICMP Packet */
    rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive GTP-U ICMP Packet */
    recvbuf = testgnb_gtpu_read(gtpu);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    ogs_pkbuf_free(recvbuf);

    /* Send UEContextReleaseRequest */
    sendbuf = testngap_build_ue_context_release_request(test_ue[i],
            NGAP_Cause_PR_radioNetwork, NGAP_CauseRadioNetwork_user_inactivity,
            true);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive UEContextReleaseCommand */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_UEContextRelease,
            test_ue[i]->ngap_procedure_code);

    /* Send UEContextReleaseComplete */
    sendbuf = testngap_build_ue_context_release_complete(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /*
     * Send Service request Using InitialUEMessage
     *  - Uplink Data Status
     */
    test_ue[i]->service_request_param.uplink_data_status = 1;
    test_ue[i]->service_request_param.
        psimask.uplink_data_status = (1 << 5 | 1 << 6);
    test_ue[i]->service_request_param.pdu_session_status = 0;
    nasbuf = testgmm_build_service_request(
            test_ue[i], OGS_NAS_SERVICE_TYPE_DATA, NULL, false, false);
    ABTS_PTR_NOTNULL(tc, nasbuf);

    test_ue[i]->service_request_param.uplink_data_status = 0;
    test_ue[i]->service_request_param.pdu_session_status = 0;
    gmmbuf = testgmm_build_service_request(
            test_ue[i], OGS_NAS_SERVICE_TYPE_DATA, nasbuf, true, false);
    ABTS_PTR_NOTNULL(tc, gmmbuf);

    sendbuf = testngap_build_initial_ue_message(test_ue[i], gmmbuf,
                NGAP_RRCEstablishmentCause_mo_Signalling, true, true);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive InitialContextSetupRequest +
     * Service accept */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_InitialContextSetup,
            test_ue[i]->ngap_procedure_code);
    ABTS_INT_EQUAL(tc, 0x0000, test_ue[i]->pdu_session_status);
    ABTS_INT_EQUAL(tc, 0x0000, test_ue[i]->pdu_session_reactivation_result);

    /* Send InitialContextSetupResponse */
    sendbuf = testngap_build_initial_context_setup_response(test_ue[i], true);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Wait to setup N3 data connection.
     * Otherwise, network-triggered service request is initiated */
    ogs_msleep(100);

    /* Send GTP-U ICMP Packet */
    rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive GTP-U ICMP Packet */
    recvbuf = testgnb_gtpu_read(gtpu);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    ogs_pkbuf_free(recvbuf);

    /* Send PDU Session release request */
    sess->ul_nas_transport_param.request_type = 0;
    sess->ul_nas_transport_param.dnn = 0;
    sess->ul_nas_transport_param.s_nssai = 0;

    sess->pdu_session_establishment_param.ssc_mode = 0;
    sess->pdu_session_establishment_param.epco = 0;

    gsmbuf = testgsm_build_pdu_session_release_request(sess);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive PDUSessionResourceReleaseCommand +
     * DL NAS transport +
     * PDU session release command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);

    /* Send PDUSessionResourceReleaseResponse */
    sendbuf = testngap_build_pdu_session_resource_release_response(sess);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send UplinkNASTransport +
     * UL NAS trasnport +
     * PDU session resource release complete */
    sess->ul_nas_transport_param.request_type = 0;
    sess->ul_nas_transport_param.dnn = 0;
    sess->ul_nas_transport_param.s_nssai = 0;

    sess->pdu_session_establishment_param.ssc_mode = 0;
    sess->pdu_session_establishment_param.epco = 0;

    gsmbuf = testgsm_build_pdu_session_release_complete(sess);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Waiting for deleting PDU session */
    ogs_msleep(100);

    /* Test Session Remove */
    test_sess_remove(sess);

    /* Send UEContextReleaseRequest */
    sendbuf = testngap_build_ue_context_release_request(test_ue[i],
            NGAP_Cause_PR_radioNetwork, NGAP_CauseRadioNetwork_user_inactivity,
            true);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive UEContextReleaseCommand */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_UEContextRelease,
            test_ue[i]->ngap_procedure_code);

    /* Send UEContextReleaseComplete */
    sendbuf = testngap_build_ue_context_release_complete(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);
    /* Send De-registration request */
    gmmbuf = testgmm_build_de_registration_request(test_ue[i], 1, true, false);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_initial_ue_message(test_ue[i], gmmbuf,
                NGAP_RRCEstablishmentCause_mo_Signalling, true, false);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);
    
    /* Receive UEContextReleaseCommand */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_UEContextRelease,
            test_ue[i]->ngap_procedure_code);
    
    /* Send UEContextReleaseComplete */
    sendbuf = testngap_build_ue_context_release_complete(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    ogs_msleep(300);
     
    }
      test_context_final_ex();

  } 
 /*AF test*/
 void muti_ue_func36(void *data)
 {  
 
 int rv;
 ogs_socknode_t *ngap;
 ogs_socknode_t *gtpu;
 ogs_pkbuf_t *gmmbuf;
 ogs_pkbuf_t *gsmbuf;
 ogs_pkbuf_t *nasbuf;
 ogs_pkbuf_t *sendbuf;
 ogs_pkbuf_t *recvbuf;
 ogs_ngap_message_t message;
 int i;
 printf("wait for app init.\r\n");
 ogs_msleep(3000);//wait for app init
 uint8_t tmp[OGS_MAX_SDU_LEN];
 char *_gtp_payload = "34ff0024"
        "0000000100000085 010002004500001c 0c0b000040015a7a 0a2d00010a2d0002"
        "00000964cd7c291f";     
 test_ue_t **test_ue;
 test_sess_t *sess = NULL;
 test_bearer_t *qos_flow = NULL;
 af_sess_t *af_sess = NULL;
 af_npcf_policyauthorization_param_t af_param;
 bson_t *doc = NULL;
 
 T_threadinfo3 *threadInfo = (T_threadinfo3 *)data;     
 abts_case *tc = threadInfo->tc;  
 test_ue = &test_ues3[threadInfo->clientIdx];  
 test_thread_context_init(threadInfo->clientIdx);
 ngap =ngaps[threadInfo->clientIdx];
 gtpu=gtpus[0];
 for (i = 0; i < g_testNum; i++) {
 
    if (i > 0)
        test_ue[i]->ran_ue_ngap_id = test_ue[i-1]->ran_ue_ngap_id;
    else
        test_ue[i]->ran_ue_ngap_id = 0;
     sess = test_sess_find_by_psi(test_ue[i], 5);
     ogs_assert(sess);        
    
     printf("threadInfo->clientIdx[%d]:test_ue[%d]\n",threadInfo->clientIdx,i);
         /* Send Registration request */
         /* Send Registration request */
    test_ue[i]->registration_request_param.guti = 1;
    gmmbuf = testgmm_build_registration_request(test_ue[i], NULL, false, false);
    ABTS_PTR_NOTNULL(tc, gmmbuf);

    test_ue[i]->registration_request_param.gmm_capability = 1;
    test_ue[i]->registration_request_param.requested_nssai = 1;
    test_ue[i]->registration_request_param.last_visited_registered_tai = 1;
    test_ue[i]->registration_request_param.ue_usage_setting = 1;
    nasbuf = testgmm_build_registration_request(test_ue[i], NULL, false, false);
    ABTS_PTR_NOTNULL(tc, nasbuf);

    sendbuf = testngap_build_initial_ue_message(test_ue[i], gmmbuf,
                NGAP_RRCEstablishmentCause_mo_Signalling, false, true);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive Identity request */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);

    /* Send Identity response */
    gmmbuf = testgmm_build_identity_response(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive Authentication request */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);

    /* Send Authentication response */
    gmmbuf = testgmm_build_authentication_response(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive Security mode command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);

    /* Send Security mode complete */
    gmmbuf = testgmm_build_security_mode_complete(test_ue[i], nasbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive InitialContextSetupRequest +
     * Registration accept */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_InitialContextSetup,
            test_ue[i]->ngap_procedure_code);

    /* Send UERadioCapabilityInfoIndication */
    sendbuf = testngap_build_ue_radio_capability_info_indication(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send InitialContextSetupResponse */
    sendbuf = testngap_build_initial_context_setup_response(test_ue[i], false);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send Registration complete */
    gmmbuf = testgmm_build_registration_complete(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive Configuration update command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);

    /* Send PDU session establishment request */
    //sess = test_sess_add_by_dnn_and_psi(test_ue[i], "internet", 5);
    sess = test_sess_find_by_psi(test_ue[i], 5);
    ogs_assert(sess);

    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_INITIAL;
    sess->ul_nas_transport_param.dnn = 1;
    sess->ul_nas_transport_param.s_nssai = 1;

    sess->pdu_session_establishment_param.ssc_mode = 1;
    sess->pdu_session_establishment_param.epco = 1;

    gsmbuf = testgsm_build_pdu_session_establishment_request(sess);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive PDUSessionResourceSetupRequest +
     * DL NAS transport +
     * PDU session establishment accept */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_PDUSessionResourceSetup,
            test_ue[i]->ngap_procedure_code);

    /* Send GTP-U ICMP Packet */
    qos_flow = test_qos_flow_find_by_qfi(sess, 1);
    ogs_assert(qos_flow);
    rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send PDUSessionResourceSetupResponse */
    sendbuf = testngap_sess_build_pdu_session_resource_setup_response(sess);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive GTP-U ICMP Packet */
    recvbuf = testgnb_gtpu_read(gtpu);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    ogs_pkbuf_free(recvbuf);

    /* Send GTP-U ICMP Packet */
    rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive GTP-U ICMP Packet */
    recvbuf = testgnb_gtpu_read(gtpu);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    ogs_pkbuf_free(recvbuf);
#if 0
    /* Send GTP-U Router Solicitation */
    rv = test_gtpu_send_slacc_rs(gtpu, qos_flow);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive GTP-U Router Advertisement */
    recvbuf = test_gtpu_read(gtpu);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testgtpu_recv(test_ue[i], recvbuf);
#endif
    /* Send PDU session establishment request */
   // sess = test_sess_add_by_dnn_and_psi(test_ue, "ims", 6);
    sess = test_sess_find_by_psi(test_ue[i], 6);
    ogs_assert(sess);

    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_INITIAL;
    sess->ul_nas_transport_param.dnn = 1;
    sess->ul_nas_transport_param.s_nssai = 1;

    sess->pdu_session_establishment_param.ssc_mode = 1;
    sess->pdu_session_establishment_param.epco = 1;

    gsmbuf = testgsm_build_pdu_session_establishment_request(sess);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive PDU session establishment accept */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);

    /* Send PDUSessionResourceSetupResponse */
    sendbuf = testngap_sess_build_pdu_session_resource_setup_response(sess);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);
//printf("111111111111111\n");
    /* Add AF-Session */
    af_sess = af_sess_add_by_ue_address(&sess->ue_ip);
    ogs_assert(af_sess);

    af_sess->supi = ogs_strdup(test_ue[i]->supi);
    ogs_assert(af_sess->supi);

    af_sess->dnn = ogs_strdup(sess->dnn);
    ogs_assert(af_sess->dnn);

    af_local_discover_and_send(
            OGS_SBI_SERVICE_TYPE_NBSF_MANAGEMENT,
            af_sess, NULL,
            af_nbsf_management_build_discover);

    /* Wait for PCF-Discovery */
    ogs_msleep(200);
   // printf("1111111111112222222\n");

    /* Send AF-Session : CREATE */
    memset(&af_param, 0, sizeof(af_param));
    af_param.med_type = OpenAPI_media_type_AUDIO;
    af_param.qos_type = 1;
    af_param.flow_type = 99; /* For ping test */

    af_local_send_to_pcf(af_sess, &af_param,
            af_npcf_policyauthorization_build_create);

    /* Receive PDUSessionResourceModifyRequest +
     * DL NAS transport +
     * PDU session modification command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_PDUSessionResourceModify,
            test_ue[i]->ngap_procedure_code);
   // printf("11111133333333333333\n");

    /* Send PDU session resource modify response */
    qos_flow = test_qos_flow_find_by_qfi(sess, 2);
    ogs_assert(qos_flow);

    sendbuf = testngap_build_qos_flow_resource_modify_response(qos_flow);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send PDU session resource modify complete */
    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
    sess->ul_nas_transport_param.dnn = 0;
    sess->ul_nas_transport_param.s_nssai = 0;

    sess->pdu_session_establishment_param.ssc_mode = 0;
    sess->pdu_session_establishment_param.epco = 0;

    gsmbuf = testgsm_build_pdu_session_modification_complete(sess);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Wait for PDU session resource modify complete */
    ogs_msleep(100);

    /* Send GTP-U ICMP Packet */
    qos_flow = test_qos_flow_find_by_qfi(sess, 2);
    ogs_assert(qos_flow);
    rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive GTP-U ICMP Packet */
    recvbuf = testgnb_gtpu_read(gtpu);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    ogs_pkbuf_free(recvbuf);

    /* Waiting for creating dedicated QoS flow in PFCP protocol */
    ogs_msleep(100);

    /* Send AF-Session : UPDATE */
    memset(&af_param, 0, sizeof(af_param));
    af_param.med_type = OpenAPI_media_type_AUDIO;
    af_param.qos_type = 2;
    af_param.flow_type = 99;

    af_local_send_to_pcf(af_sess, &af_param,
            af_npcf_policyauthorization_build_update);

    /* Receive PDUSessionResourceModifyRequest +
     * DL NAS transport +
     * PDU session modification command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_PDUSessionResourceModify,
            test_ue[i]->ngap_procedure_code);

    /* Send PDU session resource modify response */
    qos_flow = test_qos_flow_find_by_qfi(sess, 2);
    ogs_assert(qos_flow);

    sendbuf = testngap_build_qos_flow_resource_modify_response(qos_flow);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send PDU session resource modify complete */
    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
    sess->ul_nas_transport_param.dnn = 0;
    sess->ul_nas_transport_param.s_nssai = 0;

    sess->pdu_session_establishment_param.ssc_mode = 0;
    sess->pdu_session_establishment_param.epco = 0;

    gsmbuf = testgsm_build_pdu_session_modification_complete(sess);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Wait for PDU session resource modify complete */
    ogs_msleep(100);

    /* Send PDU session modification request */
    sess->pti = 8;
    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
    sess->ul_nas_transport_param.dnn = 1;
    sess->ul_nas_transport_param.s_nssai = 1;

    sess->pdu_session_establishment_param.ssc_mode = 1;
    sess->pdu_session_establishment_param.epco = 1;

    gsmbuf = testgsm_build_pdu_session_modification_request(
        qos_flow,
        0,
        OGS_NAS_QOS_CODE_MODIFY_EXISTING_QOS_RULE_AND_ADD_PACKET_FILTERS,
        OGS_NAS_MODIFY_NEW_QOS_FLOW_DESCRIPTION);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive PDUSessionResourceModifyRequest +
     * DL NAS transport +
     * PDU session modification command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_PDUSessionResourceModify,
            test_ue[i]->ngap_procedure_code);

    /* Send PDU session resource modify response */
    qos_flow = test_qos_flow_find_by_qfi(sess, 2);
    ogs_assert(qos_flow);

    sendbuf = testngap_build_qos_flow_resource_modify_response(qos_flow);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send PDU session resource modify complete */
    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
    sess->ul_nas_transport_param.dnn = 0;
    sess->ul_nas_transport_param.s_nssai = 0;

    sess->pdu_session_establishment_param.ssc_mode = 0;
    sess->pdu_session_establishment_param.epco = 0;

    gsmbuf = testgsm_build_pdu_session_modification_complete(sess);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Wait for PDU session resource modify complete */
    ogs_msleep(100);

    /* Send PDU session modification request */
    sess->pti = 9;
    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
    sess->ul_nas_transport_param.dnn = 1;
    sess->ul_nas_transport_param.s_nssai = 1;

    sess->pdu_session_establishment_param.ssc_mode = 1;
    sess->pdu_session_establishment_param.epco = 1;

    gsmbuf = testgsm_build_pdu_session_modification_request(
        qos_flow,
        0,
        OGS_NAS_QOS_CODE_MODIFY_EXISTING_QOS_RULE_AND_REPLACE_PACKET_FILTERS,
        0);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive PDUSessionResourceModifyRequest +
     * DL NAS transport +
     * PDU session modification command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_PDUSessionResourceModify,
            test_ue[i]->ngap_procedure_code);

    /* Send PDU session resource modify response */
    qos_flow = test_qos_flow_find_by_qfi(sess, 2);
    ogs_assert(qos_flow);

    sendbuf = testngap_build_qos_flow_resource_modify_response(qos_flow);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send PDU session resource modify complete */
    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
    sess->ul_nas_transport_param.dnn = 0;
    sess->ul_nas_transport_param.s_nssai = 0;

    sess->pdu_session_establishment_param.ssc_mode = 0;
    sess->pdu_session_establishment_param.epco = 0;

    gsmbuf = testgsm_build_pdu_session_modification_complete(sess);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Wait for PDU session resource modify complete */
    ogs_msleep(100);

    /* Send PDU session modification request */
    sess->pti = 10;
    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
    sess->ul_nas_transport_param.dnn = 1;
    sess->ul_nas_transport_param.s_nssai = 1;

    sess->pdu_session_establishment_param.ssc_mode = 1;
    sess->pdu_session_establishment_param.epco = 1;

    gsmbuf = testgsm_build_pdu_session_modification_request(
        qos_flow,
        OGS_5GSM_CAUSE_REGULAR_DEACTIVATION,
        OGS_NAS_QOS_CODE_MODIFY_EXISTING_QOS_RULE_AND_DELETE_PACKET_FILTERS,
        0);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive PDUSessionResourceModifyRequest +
     * DL NAS transport +
     * PDU session modification command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_PDUSessionResourceModify,
            test_ue[i]->ngap_procedure_code);

    /* Send PDU session resource modify response */
    qos_flow = test_qos_flow_find_by_qfi(sess, 2);
    ogs_assert(qos_flow);

    sendbuf = testngap_build_qos_flow_resource_modify_response(qos_flow);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send PDU session resource modify complete */
    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
    sess->ul_nas_transport_param.dnn = 0;
    sess->ul_nas_transport_param.s_nssai = 0;

    sess->pdu_session_establishment_param.ssc_mode = 0;
    sess->pdu_session_establishment_param.epco = 0;

    gsmbuf = testgsm_build_pdu_session_modification_complete(sess);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Waiting for modifying dedicated QoS flow */
    ogs_msleep(100);

    /* Send AF-Session : DELETE */
    af_local_send_to_pcf(af_sess, NULL,
            af_npcf_policyauthorization_build_delete);

    /* Receive PDUSessionResourceModifyRequest +
     * DL NAS transport +
     * PDU session modification command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_PDUSessionResourceModify,
            test_ue[i]->ngap_procedure_code);

    /* Send PDU session resource modify response */
    qos_flow = test_qos_flow_find_by_qfi(sess, 2);
    ogs_assert(qos_flow);

    sendbuf = testngap_build_qos_flow_resource_release_response(qos_flow);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send PDU session resource modify complete */
    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
    sess->ul_nas_transport_param.dnn = 0;
    sess->ul_nas_transport_param.s_nssai = 0;

    sess->pdu_session_establishment_param.ssc_mode = 0;
    sess->pdu_session_establishment_param.epco = 0;

    gsmbuf = testgsm_build_pdu_session_modification_complete(sess);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Wait for PDU session resource modify complete */
    ogs_msleep(100);

    /* Test Bearer Remove */
    test_bearer_remove(qos_flow);

    /* Send De-registration request */
    gmmbuf = testgmm_build_de_registration_request(test_ue[i], 1, true, true);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive UEContextReleaseCommand */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_UEContextRelease,
            test_ue[i]->ngap_procedure_code);

    /* Send UEContextReleaseComplete */
    sendbuf = testngap_build_ue_context_release_complete(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    ogs_msleep(300);
   }
   test_context_final_ex();

 }  
 /*test_qos_flow*/
 void muti_ue_func31(void *data)
 {  
 
 int rv;
 ogs_socknode_t *ngap;
 ogs_socknode_t *gtpu;
 ogs_pkbuf_t *gmmbuf;
 ogs_pkbuf_t *gsmbuf;
 ogs_pkbuf_t *nasbuf;
 ogs_pkbuf_t *sendbuf;
 ogs_pkbuf_t *recvbuf;
 ogs_ngap_message_t message;
 int i;
 printf("wait for app init.\r\n");
 ogs_msleep(3000);//wait for app init
 uint8_t tmp[OGS_MAX_SDU_LEN];

 char *_gtp_payload = "34ff0024"
        "0000000100000085 010002004500001c 0c0b000040015a7a 0a2d00010a2d0002"
        "00000964cd7c291f";
     
 test_ue_t **test_ue;
 test_sess_t *sess = NULL;
 test_bearer_t *qos_flow = NULL;
 af_sess_t *af_sess = NULL;
 af_npcf_policyauthorization_param_t af_param;
 bson_t *doc = NULL;
 
 T_threadinfo3 *threadInfo = (T_threadinfo3 *)data;     
 abts_case *tc = threadInfo->tc;  
 test_ue = &test_ues3[threadInfo->clientIdx];  
 test_thread_context_init(threadInfo->clientIdx);
 ngap =ngaps[threadInfo->clientIdx];
 gtpu=gtpus[0];
 for (i = 0; i < g_testNum; i++) {
 
    if (i > 0)
        test_ue[i]->ran_ue_ngap_id = test_ue[i-1]->ran_ue_ngap_id;
    else
        test_ue[i]->ran_ue_ngap_id = 0;
     sess = test_sess_find_by_psi(test_ue[i], 5);
     ogs_assert(sess);        
    
     printf("threadInfo->clientIdx[%d]:test_ue[%d]\n",threadInfo->clientIdx,i);
         /* Send Registration request */
         /* Send Registration request */
    test_ue[i]->registration_request_param.guti = 1;
    gmmbuf = testgmm_build_registration_request(test_ue[i], NULL, false, false);
    ABTS_PTR_NOTNULL(tc, gmmbuf);

    test_ue[i]->registration_request_param.gmm_capability = 1;
    test_ue[i]->registration_request_param.requested_nssai = 1;
    test_ue[i]->registration_request_param.last_visited_registered_tai = 1;
    test_ue[i]->registration_request_param.ue_usage_setting = 1;
    nasbuf = testgmm_build_registration_request(test_ue[i], NULL, false, false);
    ABTS_PTR_NOTNULL(tc, nasbuf);

    sendbuf = testngap_build_initial_ue_message(test_ue[i], gmmbuf,
                NGAP_RRCEstablishmentCause_mo_Signalling, false, true);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    

    /* Receive Identity request */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);    

    /* Send Identity response */
    gmmbuf = testgmm_build_identity_response(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive Authentication request */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);

    /* Send Authentication response */
    gmmbuf = testgmm_build_authentication_response(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive Security mode command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);

    /* Send Security mode complete */
    gmmbuf = testgmm_build_security_mode_complete(test_ue[i], nasbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive InitialContextSetupRequest +
     * Registration accept */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_InitialContextSetup,
            test_ue[i]->ngap_procedure_code);

    /* Send UERadioCapabilityInfoIndication */
    sendbuf = testngap_build_ue_radio_capability_info_indication(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);
    //printf("11111111111ssss2\n");

    /* Send InitialContextSetupResponse */
    sendbuf = testngap_build_initial_context_setup_response(test_ue[i], false);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send Registration complete */
    gmmbuf = testgmm_build_registration_complete(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive Configuration update command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);

    /* Send PDU session establishment request */
    //sess = test_sess_add_by_dnn_and_psi(test_ue, "internet", 5);
    sess = test_sess_find_by_psi(test_ue[i], 5);
    ogs_assert(sess);

    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_INITIAL;
    sess->ul_nas_transport_param.dnn = 1;
    sess->ul_nas_transport_param.s_nssai = 1;

    sess->pdu_session_establishment_param.ssc_mode = 1;
    sess->pdu_session_establishment_param.epco = 1;

    gsmbuf = testgsm_build_pdu_session_establishment_request(sess);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive PDUSessionResourceSetupRequest +
     * DL NAS transport +
     * PDU session establishment accept */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_PDUSessionResourceSetup,
            test_ue[i]->ngap_procedure_code);

    /* Send GTP-U ICMP Packet */
    qos_flow = test_qos_flow_find_by_qfi(sess, 1);
    ogs_assert(qos_flow);
    rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send PDUSessionResourceSetupResponse */
    sendbuf = testngap_sess_build_pdu_session_resource_setup_response(sess);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive GTP-U ICMP Packet */
    recvbuf = testgnb_gtpu_read(gtpu);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    ogs_pkbuf_free(recvbuf);

    /* Send GTP-U ICMP Packet */
    rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive GTP-U ICMP Packet */
    recvbuf = testgnb_gtpu_read(gtpu);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    ogs_pkbuf_free(recvbuf);

    /* Receive PDUSessionResourceModifyRequest +
     * DL NAS transport +
     * PDU session modification command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_PDUSessionResourceModify,
            test_ue[i]->ngap_procedure_code);

    /* Send PDU session resource modify response */
    qos_flow = test_qos_flow_find_by_qfi(sess, 2);
    ogs_assert(qos_flow);

    sendbuf = testngap_build_qos_flow_resource_modify_response(qos_flow);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send PDU session resource modify complete */
    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
    sess->ul_nas_transport_param.dnn = 0;
    sess->ul_nas_transport_param.s_nssai = 0;

    sess->pdu_session_establishment_param.ssc_mode = 0;
    sess->pdu_session_establishment_param.epco = 0;

    gsmbuf = testgsm_build_pdu_session_modification_complete(sess);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Waiting for creating dedicated QoS flow in PFCP protocol */
    ogs_msleep(100);

    /* Send GTP-U ICMP Packet */
    rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive GTP-U ICMP Packet */
    recvbuf = testgnb_gtpu_read(gtpu);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    /* For checking qos_flow_identifier == 2 */
   // print_buf(recvbuf->data, recvbuf->len);
    ABTS_TRUE(tc, memcmp(recvbuf->data,
        OGS_HEX(_gtp_payload, strlen(_gtp_payload), tmp), 4) == 0);
    ogs_pkbuf_free(recvbuf);

    /* Send UEContextReleaseRequest */
    sendbuf = testngap_build_ue_context_release_request(test_ue[i],
            NGAP_Cause_PR_radioNetwork, NGAP_CauseRadioNetwork_user_inactivity,
            true);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive UEContextReleaseCommand */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_UEContextRelease,
            test_ue[i]->ngap_procedure_code);

    /* Send UEContextReleaseComplete */
    sendbuf = testngap_build_ue_context_release_complete(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /*
     * Send InitialUEMessage +
     * Service request
     *  - Uplink Data Status
     */
    test_ue[i]->service_request_param.uplink_data_status = 1;
    test_ue[i]->service_request_param.psimask.uplink_data_status = 1 << sess->psi;
    test_ue[i]->service_request_param.pdu_session_status = 0;
    nasbuf = testgmm_build_service_request(
            test_ue[i], OGS_NAS_SERVICE_TYPE_DATA, NULL, false, false);
    ABTS_PTR_NOTNULL(tc, nasbuf);

    test_ue[i]->service_request_param.uplink_data_status = 0;
    test_ue[i]->service_request_param.pdu_session_status = 0;
    gmmbuf = testgmm_build_service_request(
            test_ue[i], OGS_NAS_SERVICE_TYPE_DATA, nasbuf, true, false);
    ABTS_PTR_NOTNULL(tc, gmmbuf);

    sendbuf = testngap_build_initial_ue_message(test_ue[i], gmmbuf,
                NGAP_RRCEstablishmentCause_mo_Signalling, true, true);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive InitialContextSetupRequest +
     * Service accept */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_InitialContextSetup,
            test_ue[i]->ngap_procedure_code);
    ABTS_INT_EQUAL(tc, 0x0000, test_ue[i]->pdu_session_status);
    ABTS_INT_EQUAL(tc, 0x0000, test_ue[i]->pdu_session_reactivation_result);
//printf("sssssssssssssssss2\n");
    /* Send InitialContextSetupResponse */
    sendbuf = testngap_build_initial_context_setup_response(test_ue[i], true);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Wait to setup N3 data connection.
     * Otherwise, network-triggered service request is initiated */
    ogs_msleep(100);

    /* Send GTP-U ICMP Packet */
    rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive GTP-U ICMP Packet */
    recvbuf = testgnb_gtpu_read(gtpu);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    /* For checking qos_flow_identifier == 2 */
   // print_buf(recvbuf->data, recvbuf->len);
    ABTS_TRUE(tc, memcmp(recvbuf->data,
        OGS_HEX(_gtp_payload, strlen(_gtp_payload), tmp), 4) == 0);
    ogs_pkbuf_free(recvbuf);
    
    /* Send De-registration request */
    gmmbuf = testgmm_build_de_registration_request(test_ue[i], 1, true, true);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive UEContextReleaseCommand */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_UEContextRelease,
            test_ue[i]->ngap_procedure_code);

    /* Send UEContextReleaseComplete */
    sendbuf = testngap_build_ue_context_release_complete(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    ogs_msleep(300);
   }
   test_context_final_ex();

 }    
 void muti_ue_func37(void *data)
 {  
 
 int rv;
 ogs_socknode_t *ngap;
 ogs_socknode_t *gtpu;
 ogs_pkbuf_t *gmmbuf;
 ogs_pkbuf_t *gsmbuf;
 ogs_pkbuf_t *nasbuf;
 ogs_pkbuf_t *sendbuf;
 ogs_pkbuf_t *recvbuf;
 ogs_ngap_message_t message;
 int i;
 printf("wait for app init.\r\n");
 ogs_msleep(3000);//wait for app init
 uint8_t tmp[OGS_MAX_SDU_LEN];
 char *_gtp_payload = "34ff0024"
        "0000000100000085 010002004500001c 0c0b000040015a7a 0a2d00010a2d0002"
        "00000964cd7c291f";     
 test_ue_t **test_ue;
 test_sess_t *sess = NULL;
 test_bearer_t *qos_flow = NULL;
 af_sess_t *af_sess = NULL;
 af_npcf_policyauthorization_param_t af_param;
 bson_t *doc = NULL;
 
 T_threadinfo3 *threadInfo = (T_threadinfo3 *)data;     
 abts_case *tc = threadInfo->tc;  
 test_ue = &test_ues3[threadInfo->clientIdx];  
 test_thread_context_init(threadInfo->clientIdx);
 ngap =ngaps[threadInfo->clientIdx];
 gtpu=gtpus[0];
 for (i = 0; i < g_testNum; i++) {
 
    if (i > 0)
        test_ue[i]->ran_ue_ngap_id = test_ue[i-1]->ran_ue_ngap_id;
    else
        test_ue[i]->ran_ue_ngap_id = 0;
     sess = test_sess_find_by_psi(test_ue[i], 5);
     ogs_assert(sess);        
    
     printf("threadInfo->clientIdx[%d]:test_ue[%d]\n",threadInfo->clientIdx,i);
         /* Send Registration request */
         /* Send Registration request */
    test_ue[i]->registration_request_param.guti = 1;
    gmmbuf = testgmm_build_registration_request(test_ue[i], NULL, false, false);
    ABTS_PTR_NOTNULL(tc, gmmbuf);

    test_ue[i]->registration_request_param.gmm_capability = 1;
    test_ue[i]->registration_request_param.requested_nssai = 1;
    test_ue[i]->registration_request_param.last_visited_registered_tai = 1;
    test_ue[i]->registration_request_param.ue_usage_setting = 1;
    nasbuf = testgmm_build_registration_request(test_ue[i], NULL, false, false);
    ABTS_PTR_NOTNULL(tc, nasbuf);

    sendbuf = testngap_build_initial_ue_message(test_ue[i], gmmbuf,
                NGAP_RRCEstablishmentCause_mo_Signalling, false, true);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive Identity request */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);

    /* Send Identity response */
    gmmbuf = testgmm_build_identity_response(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive Authentication request */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);

    /* Send Authentication response */
    gmmbuf = testgmm_build_authentication_response(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive Security mode command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);

    /* Send Security mode complete */
    gmmbuf = testgmm_build_security_mode_complete(test_ue[i], nasbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive InitialContextSetupRequest +
     * Registration accept */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_InitialContextSetup,
            test_ue[i]->ngap_procedure_code);

    /* Send UERadioCapabilityInfoIndication */
    sendbuf = testngap_build_ue_radio_capability_info_indication(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send InitialContextSetupResponse */
    sendbuf = testngap_build_initial_context_setup_response(test_ue[i], false);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send Registration complete */
    gmmbuf = testgmm_build_registration_complete(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive Configuration update command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);

    /* Send PDU session establishment request */
    //sess = test_sess_add_by_dnn_and_psi(test_ue, "internet", 5);
    sess = test_sess_find_by_psi(test_ue[i], 5);
    ogs_assert(sess);

    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_INITIAL;
    sess->ul_nas_transport_param.dnn = 1;
    sess->ul_nas_transport_param.s_nssai = 1;

    sess->pdu_session_establishment_param.ssc_mode = 1;
    sess->pdu_session_establishment_param.epco = 1;

    gsmbuf = testgsm_build_pdu_session_establishment_request(sess);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive PDUSessionResourceSetupRequest +
     * DL NAS transport +
     * PDU session establishment accept */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_PDUSessionResourceSetup,
            test_ue[i]->ngap_procedure_code);

    /* Send GTP-U ICMP Packet */
    qos_flow = test_qos_flow_find_by_qfi(sess, 1);
    ogs_assert(qos_flow);
    rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send PDUSessionResourceSetupResponse */
    sendbuf = testngap_sess_build_pdu_session_resource_setup_response(sess);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive GTP-U ICMP Packet */
    recvbuf = testgnb_gtpu_read(gtpu);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    ogs_pkbuf_free(recvbuf);

    /* Send GTP-U ICMP Packet */
    rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive GTP-U ICMP Packet */
    recvbuf = testgnb_gtpu_read(gtpu);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    ogs_pkbuf_free(recvbuf);
#if 0
    /* Send GTP-U Router Solicitation */
    rv = test_gtpu_send_slacc_rs(gtpu, qos_flow);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive GTP-U Router Advertisement */
    recvbuf = test_gtpu_read(gtpu);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testgtpu_recv(test_ue[i], recvbuf);
#endif
    /* Send PDU session establishment request */
    //sess = test_sess_add_by_dnn_and_psi(test_ue, "ims", 6);
    sess = test_sess_find_by_psi(test_ue[i], 6);
    ogs_assert(sess);

    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_INITIAL;
    sess->ul_nas_transport_param.dnn = 1;
    sess->ul_nas_transport_param.s_nssai = 1;

    sess->pdu_session_establishment_param.ssc_mode = 1;
    sess->pdu_session_establishment_param.epco = 1;

    gsmbuf = testgsm_build_pdu_session_establishment_request(sess);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive PDU session establishment accept */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);

    /* Send PDUSessionResourceSetupResponse */
    sendbuf = testngap_sess_build_pdu_session_resource_setup_response(sess);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Add AF-Session */
    af_sess = af_sess_add_by_ue_address(&sess->ue_ip);
    ogs_assert(af_sess);

    af_sess->supi = ogs_strdup(test_ue[i]->supi);
    ogs_assert(af_sess->supi);

    af_sess->dnn = ogs_strdup(sess->dnn);
    ogs_assert(af_sess->dnn);

    af_local_discover_and_send(
            OGS_SBI_SERVICE_TYPE_NBSF_MANAGEMENT,
            af_sess, NULL,
            af_nbsf_management_build_discover);

    /* Wait for PCF-Discovery */
    ogs_msleep(100);

    /* Send AF-Session : CREATE */
    memset(&af_param, 0, sizeof(af_param));
    af_local_send_to_pcf(af_sess, &af_param,
            af_npcf_policyauthorization_build_create_video);

    /* Receive PDUSessionResourceModifyRequest +
     * DL NAS transport +
     * PDU session modification command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_PDUSessionResourceModify,
            test_ue[i]->ngap_procedure_code);

    /* Send PDU session resource modify response */
    ogs_list_init(&sess->qos_flow_to_modify_list);

    qos_flow = test_qos_flow_find_by_qfi(sess, 2);
    ogs_assert(qos_flow);
    ogs_list_add(&sess->qos_flow_to_modify_list, &qos_flow->to_modify_node);

    qos_flow = test_qos_flow_find_by_qfi(sess, 3);
    ogs_assert(qos_flow);
    ogs_list_add(&sess->qos_flow_to_modify_list, &qos_flow->to_modify_node);

    sendbuf = testngap_sess_build_pdu_session_resource_modify_response(sess);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send PDU session resource modify complete */
    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
    sess->ul_nas_transport_param.dnn = 0;
    sess->ul_nas_transport_param.s_nssai = 0;

    sess->pdu_session_establishment_param.ssc_mode = 0;
    sess->pdu_session_establishment_param.epco = 0;

    gsmbuf = testgsm_build_pdu_session_modification_complete(sess);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Wait for PDU session resource modify complete */
    ogs_msleep(100);

    /* Send PDU session modification request */
    qos_flow = test_qos_flow_find_by_qfi(sess, 2);
    ogs_assert(qos_flow);

    sess->pti = 8;
    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
    sess->ul_nas_transport_param.dnn = 1;
    sess->ul_nas_transport_param.s_nssai = 1;

    sess->pdu_session_establishment_param.ssc_mode = 1;
    sess->pdu_session_establishment_param.epco = 1;

    gsmbuf = testgsm_build_pdu_session_modification_request(
        qos_flow,
        0,
        OGS_NAS_QOS_CODE_MODIFY_EXISTING_QOS_RULE_AND_DELETE_PACKET_FILTERS,
        0);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive PDUSessionResourceModifyRequest +
     * DL NAS transport +
     * PDU session modification command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_PDUSessionResourceModify,
            test_ue[i]->ngap_procedure_code);

    /* Send PDU session resource modify response */
    qos_flow = test_qos_flow_find_by_qfi(sess, 2);
    ogs_assert(qos_flow);

    sendbuf = testngap_build_qos_flow_resource_release_response(qos_flow);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send PDU session resource modify complete */
    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
    sess->ul_nas_transport_param.dnn = 0;
    sess->ul_nas_transport_param.s_nssai = 0;

    sess->pdu_session_establishment_param.ssc_mode = 0;
    sess->pdu_session_establishment_param.epco = 0;

    gsmbuf = testgsm_build_pdu_session_modification_complete(sess);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Wait for PDU session resource modify complete */
    ogs_msleep(100);

    /* Test Bearer Remove */
    test_bearer_remove(qos_flow);

    /* Send AF-Session : DELETE */
    af_local_send_to_pcf(af_sess, NULL,
            af_npcf_policyauthorization_build_delete);

    /* Receive PDUSessionResourceModifyRequest +
     * DL NAS transport +
     * PDU session modification command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_PDUSessionResourceModify,
            test_ue[i]->ngap_procedure_code);

    /* Send PDU session resource modify response */
    qos_flow = test_qos_flow_find_by_qfi(sess, 3);
    ogs_assert(qos_flow);

    sendbuf = testngap_build_qos_flow_resource_release_response(qos_flow);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Send PDU session resource modify complete */
    sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
    sess->ul_nas_transport_param.dnn = 0;
    sess->ul_nas_transport_param.s_nssai = 0;

    sess->pdu_session_establishment_param.ssc_mode = 0;
    sess->pdu_session_establishment_param.epco = 0;

    gsmbuf = testgsm_build_pdu_session_modification_complete(sess);
    ABTS_PTR_NOTNULL(tc, gsmbuf);
    gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Wait for PDU session resource modify complete */
    ogs_msleep(100);

    /* Test Bearer Remove */
    test_bearer_remove(qos_flow);

    /* Send De-registration request */
    gmmbuf = testgmm_build_de_registration_request(test_ue[i], 1, true, true);
    ABTS_PTR_NOTNULL(tc, gmmbuf);
    sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive UEContextReleaseCommand */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_UEContextRelease,
            test_ue[i]->ngap_procedure_code);

    /* Send UEContextReleaseComplete */
    sendbuf = testngap_build_ue_context_release_complete(test_ue[i]);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    ogs_msleep(300);
    }
      test_context_final_ex();

    }  

abts_suite *test_vonr_multi_ue_test(abts_suite *suite)
{

    time_t curtime;
    time(&curtime);

    suite = ADD_SUITE(suite);
    int i;
    for(i=0;i<g_testcycleNum;i++){
        printf("\r\n");
        printf(">>>>>>>>>>>>>>>>>g_testcycleNum:%d:%d Time:%s\r\n",g_testcycleNum,i,ctime(&curtime));
        abts_run_test(suite, muti_ue_threads, NULL);
        }
    return suite;
}
