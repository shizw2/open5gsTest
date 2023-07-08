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



extern int g_testNum;
extern int g_threadNum;

//#define NUM_OF_TEST_UE 100

static ogs_thread_t *mthread3;
 //void muti_ue_func(void *data);
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



test_ue_t *test_ues3[MAX_THREAD][10000];
ogs_socknode_t *ngaps[MAX_THREAD];
ogs_socknode_t *gtpus[2];

const struct testcaselist3 {
    void *(*func)(void *data);
} casetests3[] = {
    {muti_ue_func31},
    {muti_ue_func32},
    {muti_ue_func33},
    {muti_ue_func34},
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
	if  (g_threadNum > 4)
	{
		iPthreadnum3 = 4;
        printf("MAX thread num is 4!\n");
	}
	
    /* Two gNB connects to UPF */
    gtpus[0] = test_gtpu_server(1, AF_INET);
    ABTS_PTR_NOTNULL(tc, gtpus[0]);

    gtpus[1] = test_gtpu_server(2, AF_INET);
    ABTS_PTR_NOTNULL(tc, gtpus[1]);
	struct timeval start_time, stop_time;
    gettimeofday(&start_time, NULL);
	
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
			mobile_identity_suci.protection_scheme_id = OGS_PROTECTION_SCHEME_NULL;
			mobile_identity_suci.home_network_pki_value = 0;

			imsi_index = iTmp*10000+ i + 1;		

			char scheme_output[11]={0};
			ogs_snprintf(scheme_output,sizeof(scheme_output),"%010lu",imsi_index);
			test_ues3[iTmp][i] = test_ue_add_by_suci(&mobile_identity_suci, scheme_output);
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
			if(iTmp==3){
			doc = test_db_new_qos_flow(test_ues3[iTmp][i]);
             }else{
            doc = test_db_new_session(test_ues3[iTmp][i]);
                }

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
            sess = test_sess_add_by_dnn_and_psi(test_ues3[iTmp][i], "ims", 6);
            ogs_assert(sess);
        }
       
    }
    #if 1
    for (iTmp = 0; iTmp < iPthreadnum3; iTmp++)
    {
        ngaps[iTmp] =  testngap_client_n(AF_INET,iTmp);
        ogs_assert(ngaps[iTmp]);
  
            /* Send NG-Setup Reqeust */
        sendbuf = testngap_build_ng_setup_request(gnb_id+iTmp, 28);
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
/* muti_ue*/
 void muti_ue_func32(void *data)
{
    int rv;
    ogs_socknode_t *ngap;
   // ogs_socknode_t *gtpu;
    ogs_pkbuf_t *gmmbuf;
    ogs_pkbuf_t *gsmbuf;
    ogs_pkbuf_t *nasbuf;
    ogs_pkbuf_t *sendbuf;
    ogs_pkbuf_t *recvbuf;
    ogs_ngap_message_t message;
    int i;
    printf("wait for app init.\r\n");
    ogs_msleep(3000);//wait for app init
    struct timeval start_time, stop_time;
    gettimeofday(&start_time, NULL);

    ogs_nas_5gs_mobile_identity_suci_t mobile_identity_suci;
    //test_ue_t *test_ue[MAX_UE];
	test_ue_t **test_ue;
    test_sess_t *sess = NULL;
    test_bearer_t *qos_flow = NULL;

    bson_t *doc = NULL;

    T_threadinfo3 *threadInfo = (T_threadinfo3 *)data;
    
    abts_case *tc = threadInfo->tc;

    //printf("clientIdx:%d.\r\n",threadInfo->clientIdx);

    test_ue = &test_ues3[threadInfo->clientIdx];

    test_thread_context_init(threadInfo->clientIdx);//thread
	
    /* gNB connects to AMF */
    //ngap = testngap_client(AF_INET);
    //ngap =  testngap_client_n(AF_INET,threadInfo->clientIdx);
    //ABTS_PTR_NOTNULL(tc, ngap);
    ngap =ngaps[threadInfo->clientIdx];

    /* gNB connects to UPF */
   // gtpu = test_gtpu_server(1, AF_INET);
  //  ABTS_PTR_NOTNULL(tc, gtpu);
#if 0
    /* Send NG-Setup Reqeust */
    sendbuf = testngap_build_ng_setup_request(0x4000, 22);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive NG-Setup Response */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
#endif

    for (i = 0; i < g_testNum; i++) {

        if (i > 0)
            test_ue[i]->ran_ue_ngap_id = test_ue[i-1]->ran_ue_ngap_id;
        else
            test_ue[i]->ran_ue_ngap_id = 0;
#if 0
        /* Send PDU session establishment request */
        sess = test_sess_add_by_dnn_and_psi(test_ue[i], "internet", 5);
        ogs_assert(sess);
#endif

        /********** Insert Subscriber in Database */
        //doc = test_db_new_simple(test_ue[i]);
        //ABTS_PTR_NOTNULL(tc, doc);
        //ABTS_INT_EQUAL(tc, OGS_OK, test_db_insert_ue(test_ue[i], doc));
		printf("threadInfo->clientIdx[%d]:test_ue[%d]\n",threadInfo->clientIdx,i);
        sess = test_sess_find_by_psi(test_ue[i], 5);
        ogs_assert(sess);
	
        /* Send Registration request */
        test_ue[i]->registration_request_param.guti = 1;
        gmmbuf = testgmm_build_registration_request(test_ue[i], NULL, false, false);
        ABTS_PTR_NOTNULL(tc, gmmbuf);

        test_ue[i]->registration_request_param.gmm_capability = 1;
        test_ue[i]->registration_request_param.s1_ue_network_capability = 1;
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
		//printf("tttttttttttt   rv=%d\n",rv);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);

        /* Receive Identity request */
        recvbuf = testgnb_ngap_read(ngap);
        ABTS_PTR_NOTNULL(tc, recvbuf);
        testngap_recv(test_ue[i], recvbuf);
		//printf("Receive Identity request\n");

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
		//printf("Authentication request\n");

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
		//printf("Receive Security mode command\n");

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
		//printf("InitialContextSetupRequest\n");

        /* Send UERadioCapabilityInfoIndication*/
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
		//printf("Configuration update command\n");
#if 0

        sess->ul_nas_transport_param.request_type =
            OGS_NAS_5GS_REQUEST_TYPE_INITIAL;
        sess->ul_nas_transport_param.dnn = 1;
        sess->ul_nas_transport_param.s_nssai = 0;

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

		//printf("Receive PDUSessionResourceSetupRequest begin\n");

        /* Receive PDUSessionResourceSetupRequest +
         * DL NAS transport +
         * PDU session establishment accept */
        recvbuf = testgnb_ngap_read(ngap);
        //print_buf(recvbuf->data, recvbuf->len);
       
        ABTS_PTR_NOTNULL(tc, recvbuf);
        //print_buf(recvbuf->data, recvbuf->len);
        testngap_recv(test_ue[i], recvbuf);
       
        ABTS_INT_EQUAL(tc,
                NGAP_ProcedureCode_id_PDUSessionResourceSetup,
                test_ue[i]->ngap_procedure_code);
		//printf("Receive PDUSessionResourceSetupRequest end\n");

        /* Send PDUSessionResourceSetupResponse */
        sendbuf = testngap_sess_build_pdu_session_resource_setup_response(sess);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);

#endif

    }

#if 1
   

	
	for (i = 0; i < g_testNum; i++) {
        ogs_msleep(50);
		/* Send PDU session establishment request */
        sess = test_sess_find_by_psi(test_ue[i], 5);
        ogs_assert(sess);
		//printf("sess->dnn:%s\n",sess->dnn);
		sess->ul_nas_transport_param.request_type =
        OGS_NAS_5GS_REQUEST_TYPE_INITIAL;
        sess->ul_nas_transport_param.dnn = 1;
        sess->ul_nas_transport_param.s_nssai = 0;

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
        //printf("Receive PDUSessionResourceSetupRequest\n");

        /* Send PDUSessionResourceSetupResponse */
        sendbuf = testngap_sess_build_pdu_session_resource_setup_response(sess);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
	}
#endif	




    for (i = 0; i < g_testNum; i++) {
        ogs_msleep(50);
        /* Send PDU session establishment request */
        sess = test_sess_find_by_psi(test_ue[i], 5);
        ogs_assert(sess);

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
        ABTS_INT_EQUAL(tc,
                NGAP_ProcedureCode_id_PDUSessionResourceRelease,
                test_ue[i]->ngap_procedure_code);

       // printf("DUSessionResourceReleaseCommand\n");

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

        test_bearer_remove_all(sess);//bear改为了线程级别 直接这里释放
    }




    for (i = 0; i < g_testNum; i++) {
        ogs_msleep(50);
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
        //printf("Receive UEContextReleaseCommand\n");

        /* Send UEContextReleaseComplete */
        sendbuf = testngap_build_ue_context_release_complete(test_ue[i]);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
    }

	


    ogs_msleep(500);


    /* gNB disonncect from UPF */
   // testgnb_gtpu_close(gtpu);

    /* gNB disonncect from AMF */
    //testgnb_ngap_close(ngap);

    /* Clear Test UE Context */
    //test_ue_remove_all();

    test_context_final_ex();

    printf("test2  end \n");


}
 
 /*  vonr_session_test2_func*/

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
     ogs_msleep(4000);//wait for app init
     struct timeval start_time, stop_time;
   
 
     ogs_nas_5gs_mobile_identity_suci_t mobile_identity_suci;
     //test_ue_t *test_ue[MAX_UE];
     test_ue_t **test_ue;
     test_sess_t *sess = NULL;
     test_bearer_t *qos_flow = NULL;
 
     bson_t *doc = NULL;
     uint8_t tmp[OGS_MAX_SDU_LEN];
     char *_gtp_payload = "34ff0024"
        "0000000100000085 010002004500001c 0c0b000040015a7a 0a2d00010a2d0002"
        "00000964cd7c291f";
     T_threadinfo3 *threadInfo = (T_threadinfo3 *)data;
     
     abts_case *tc = threadInfo->tc;
 
     //printf("clientIdx:%d.\r\n",threadInfo->clientIdx);
 
     test_ue = &test_ues3[threadInfo->clientIdx];
 
     test_thread_context_init(threadInfo->clientIdx);//thread
     
     /* gNB connects to AMF */
     //ngap = testngap_client(AF_INET);
     //ngap =  testngap_client_n(AF_INET,threadInfo->clientIdx);
     //ABTS_PTR_NOTNULL(tc, ngap);
     ngap =ngaps[threadInfo->clientIdx];
     gtpu =gtpus[0];
     /* gNB connects to UPF */
     //gtpu = test_gtpu_server(1, AF_INET);
    // ABTS_PTR_NOTNULL(tc, gtpu);
 #if 0
     /* Send NG-Setup Reqeust */
     sendbuf = testngap_build_ng_setup_request(0x4001, 22);
     ABTS_PTR_NOTNULL(tc, sendbuf);
     rv = testgnb_ngap_send(ngap, sendbuf);
     ABTS_INT_EQUAL(tc, OGS_OK, rv);
 
     /* Receive NG-Setup Response */
     recvbuf = testgnb_ngap_read(ngap);
     ABTS_PTR_NOTNULL(tc, recvbuf);
 
#endif    
   
 
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
    test_ue[i]->registration_request_param.s1_ue_network_capability = 1;
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
    //printf("Receive Identity request\n");

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
    //printf("Receive Authentication request\n");

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
    //printf("Receive Configuration update command\n");

    /* Send PDU session establishment request */
    //sess = test_sess_add_by_dnn_and_psi(test_ue[i], "internet", 5);
    sess = test_sess_find_by_psi(test_ue[i], 5);
    ogs_assert(sess);
   // printf("sess sess->dnn:%s\n",sess->dnn);

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
    //printf("Receive PDUSessionResourceSetupRequest\n");
#if 1    /* Send GTP-U ICMP Packet */
    qos_flow = test_qos_flow_find_by_qfi(sess, 1);
    ogs_assert(qos_flow);
    rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);
#endif
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
    //sess = test_sess_add_by_dnn_and_psi(test_ue[i], "ims", 6);
    sess = test_sess_find_by_psi(test_ue[i], 6);
    ogs_assert(sess);
   // printf("sess6 sess->dnn:%s\n",sess->dnn);

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
   // printf("sess61 sess->dnn:%s\n",sess->dnn);

    /* Receive PDU session establishment accept */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
        NGAP_ProcedureCode_id_PDUSessionResourceSetup,
        test_ue[i]->ngap_procedure_code);
    //printf("sess60 sess->dnn:%s\n",sess->dnn);

    /* Send PDUSessionResourceSetupResponse */
    sendbuf = testngap_sess_build_pdu_session_resource_setup_response(sess);
    //printf("sess6030 sess->dnn:%s\n",sess->dnn);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    //printf("sess600 sess->dnn:%s\n",sess->dnn);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);
    //printf("sess62 sess->dnn:%s\n",sess->dnn);

    /* Receive PDU session modification command */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);

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
#if 1
    /* Send GTP-U ICMP Packet */
    rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive GTP-U ICMP Packet */
    recvbuf = testgnb_gtpu_read(gtpu);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    ogs_pkbuf_free(recvbuf);
#endif
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
    //printf("Receive UEContextReleaseCommand\n");

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
#if 1

    /* Send GTP-U ICMP Packet */
    rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);
#endif
    /* Send InitialContextSetupResponse */
    sendbuf = testngap_build_initial_context_setup_response(test_ue[i], true);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);
#if 1

    /* Receive GTP-U ICMP Packet */
    recvbuf = testgnb_gtpu_read(gtpu);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    ogs_pkbuf_free(recvbuf);
#endif
#if OGS_SBI_DISABLE_NETWORK_SERVICE_REQUEST_WHILE_ACTIVATING == 0

    /* Receive PDUSessionResourceSetupRequest */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    testngap_recv(test_ue[i], recvbuf);
    ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_PDUSessionResourceSetup,
            test_ue[i]->ngap_procedure_code);

    /* Send PDUSessionResourceSetupResponse */
    sendbuf = testngap_sess_build_pdu_session_resource_setup_response(sess);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);
 #endif
#if 1

    /* Send GTP-U ICMP Packet */
    rv = test_gtpu_send_ping(gtpu, qos_flow, TEST_PING_IPV4);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive GTP-U ICMP Packet */
    recvbuf = testgnb_gtpu_read(gtpu);
    ABTS_PTR_NOTNULL(tc, recvbuf);
    ogs_pkbuf_free(recvbuf);
#endif

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
     
        }
     ogs_msleep(500);
 
 
     /* gNB disonncect from UPF */
     //testgnb_gtpu_close(gtpu);
 
     /* gNB disonncect from AMF */
    // testgnb_ngap_close(ngap);
 
     /* Clear Test UE Context */
     //test_ue_remove_all();
 
      test_context_final_ex();
     printf("test3  end \n");
 
 }
 /*  test_gmm_status*/
 void muti_ue_func31(void *data)
  {
      int rv;
      ogs_socknode_t *ngap;
     // ogs_socknode_t *gtpu;
      ogs_pkbuf_t *gmmbuf;
      ogs_pkbuf_t *gsmbuf;
      ogs_pkbuf_t *nasbuf;
      ogs_pkbuf_t *sendbuf;
      ogs_pkbuf_t *recvbuf;
      ogs_ngap_message_t message;
      int i;
      printf("wait for app init.\r\n");
      ogs_msleep(3000);//wait for app init
      struct timeval start_time, stop_time;
      gettimeofday(&start_time, NULL);
  
      ogs_nas_5gs_mobile_identity_suci_t mobile_identity_suci;
      //test_ue_t *test_ue[MAX_UE];
      test_ue_t **test_ue;
      test_sess_t *sess = NULL;
      test_bearer_t *qos_flow = NULL;
  
      bson_t *doc = NULL;
  
      T_threadinfo3 *threadInfo = (T_threadinfo3 *)data;     
      abts_case *tc = threadInfo->tc;  
      test_ue = &test_ues3[threadInfo->clientIdx];  
      test_thread_context_init(threadInfo->clientIdx);
      ngap =ngaps[threadInfo->clientIdx];
      for (i = 0; i < g_testNum; i++) {
 
         if (i > 0)
             test_ue[i]->ran_ue_ngap_id = test_ue[i-1]->ran_ue_ngap_id;
         else
             test_ue[i]->ran_ue_ngap_id = 0;
          sess = test_sess_find_by_psi(test_ue[i], 5);
          ogs_assert(sess);        
         
          printf("threadInfo->clientIdx[%d]:test_ue[%d]\n",threadInfo->clientIdx,i);

          
          /* Send Registration request */
          gmmbuf = testgmm_build_registration_request(test_ue[i], NULL, false, false);
          ABTS_PTR_NOTNULL(tc, gmmbuf);
          
          test_ue[i]->registration_request_param.gmm_capability = 1;
          test_ue[i]->registration_request_param.s1_ue_network_capability = 1;
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
          
          /*
           * Related to issue #536. When running with VirtualBox 1 Core,
           * AMF sends namf-callback response very late. In SMF,
           * the Session context has already been removed, so an assertion occurs.
           *
           * It seems to be related to the response part of MHD. We will check
           * if the same situation occurs after upgrading to nghttp2.
           *
           * If this issue still occurs on nghttp2,
           * I will remove the assertion from SMF.
           */
          ogs_msleep(100);
          
          /* Send GMM Status */
          gmmbuf = testgmm_build_gmm_status(test_ue,
                  OGS_5GMM_CAUSE_MESSAGE_NOT_COMPATIBLE_WITH_THE_PROTOCOL_STATE);
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
        }
      ogs_msleep(100);
      test_context_final_ex();
      printf("test1  end \n");
    }  
 void muti_ue_func34(void *data)
  {
      int rv;
      ogs_socknode_t *ngap1,*ngap2;
      ogs_socknode_t *gtpu1,*gtpu2;
      ogs_pkbuf_t *gmmbuf;
      ogs_pkbuf_t *gsmbuf;
      ogs_pkbuf_t *nasbuf;
      ogs_pkbuf_t *sendbuf;
      ogs_pkbuf_t *recvbuf;
      ogs_ngap_message_t message;
      int i;
      printf("wait for app init.\r\n");
      ogs_msleep(3000);//wait for app init
      struct timeval start_time, stop_time;
      gettimeofday(&start_time, NULL);
  
      //ogs_nas_5gs_mobile_identity_suci_t mobile_identity_suci;
      //test_ue_t *test_ue[MAX_UE];
      test_ue_t **test_ue;
      test_sess_t *sess = NULL;
      test_bearer_t *qos_flow = NULL;
      uint8_t tmp[OGS_MAX_SDU_LEN];
      char *_gtp_payload = "34ff0024"
        "0000000100000085 010002004500001c 0c0b000040015a7a 0a2d00010a2d0002"
        "00000964cd7c291f";
      bson_t *doc = NULL;
  
      T_threadinfo3 *threadInfo = (T_threadinfo3 *)data;     
      abts_case *tc = threadInfo->tc;  
      test_ue = &test_ues3[threadInfo->clientIdx];  
      test_thread_context_init(threadInfo->clientIdx);
      ngap1 =ngaps[threadInfo->clientIdx];
      ngap2 =ngaps[threadInfo->clientIdx];
      #if 0
      /* Two gNB connects to AMF */
         ngap1 = testngap_client(AF_INET);
         ABTS_PTR_NOTNULL(tc, ngap1);
      
         ngap2 = testngap_client(AF_INET);
         ABTS_PTR_NOTNULL(tc, ngap2);
      #endif
      gtpu1=gtpus[0];
      gtpu2=gtpus[1];
      for (i = 0; i < g_testNum; i++) {
 
         if (i > 0)
             test_ue[i]->ran_ue_ngap_id = test_ue[i-1]->ran_ue_ngap_id;
         else
             test_ue[i]->ran_ue_ngap_id = 0;
         sess = test_sess_find_by_psi(test_ue[i], 5);
         ogs_assert(sess);
         printf("threadInfo->clientIdx[%d]:test_ue[%d]\n",threadInfo->clientIdx,i);
            /* Two gNB connects to AMF */
            
#if 0
            /* NG-Setup Reqeust/Response for Source gNB */
            sendbuf = testngap_build_ng_setup_request(0x4000, 28);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap1, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            recvbuf = testgnb_ngap_read(ngap1);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);
#endif

            /* NG-Setup Reqeust/Response for Target gNB */
            sendbuf = testngap_build_ng_setup_request(0x4001, 28);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap2, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            recvbuf = testgnb_ngap_read(ngap2);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);
#if 0

            doc = test_db_new_qos_flow(test_ue[i]);
            ABTS_PTR_NOTNULL(tc, doc);
            ABTS_INT_EQUAL(tc, OGS_OK, test_db_insert_ue(test_ue[i], doc));

#endif

            /* Send Registration request */
            test_ue[i]->registration_request_param.guti = 1;
            gmmbuf = testgmm_build_registration_request(test_ue[i], NULL, false, false);
            ABTS_PTR_NOTNULL(tc, gmmbuf);

            test_ue[i]->registration_request_param.gmm_capability = 1;
            test_ue[i]->registration_request_param.s1_ue_network_capability = 1;
            test_ue[i]->registration_request_param.requested_nssai = 1;
            test_ue[i]->registration_request_param.last_visited_registered_tai = 1;
            test_ue[i]->registration_request_param.ue_usage_setting = 1;
            nasbuf = testgmm_build_registration_request(test_ue[i], NULL, false, false);
            ABTS_PTR_NOTNULL(tc, nasbuf);

            sendbuf = testngap_build_initial_ue_message(test_ue[i], gmmbuf,
            NGAP_RRCEstablishmentCause_mo_Signalling, false, true);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap1, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            /* Receive Identity request */
            recvbuf = testgnb_ngap_read(ngap1);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);

            /* Send Identity response */
            gmmbuf = testgmm_build_identity_response(test_ue[i]);
            ABTS_PTR_NOTNULL(tc, gmmbuf);
            sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap1, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            /* Receive Authentication request */
            recvbuf = testgnb_ngap_read(ngap1);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);

            /* Send Authentication response */
            gmmbuf = testgmm_build_authentication_response(test_ue[i]);
            ABTS_PTR_NOTNULL(tc, gmmbuf);
            sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap1, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            /* Receive Security mode command */
            recvbuf = testgnb_ngap_read(ngap1);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);

            /* Send Security mode complete */
            gmmbuf = testgmm_build_security_mode_complete(test_ue[i], nasbuf);
            ABTS_PTR_NOTNULL(tc, gmmbuf);
            sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap1, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
            //printf("44444444444  Send Security mode complete \n");

            /* Receive InitialContextSetupRequest +
            * Registration accept */
            recvbuf = testgnb_ngap_read(ngap1);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);
            ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_InitialContextSetup,
            test_ue[i]->ngap_procedure_code);

            /* Send UERadioCapabilityInfoIndication */
            sendbuf = testngap_build_ue_radio_capability_info_indication(test_ue[i]);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap1, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            /* Send InitialContextSetupResponse */
            sendbuf = testngap_build_initial_context_setup_response(test_ue[i], false);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap1, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
            //printf("44444444444  Send InitialContextSetupResponse \n");

            /* Send Registration complete */
            gmmbuf = testgmm_build_registration_complete(test_ue[i]);
            ABTS_PTR_NOTNULL(tc, gmmbuf);
            sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap1, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            /* Receive Configuration update command */
            recvbuf = testgnb_ngap_read(ngap1);
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
            rv = testgnb_ngap_send(ngap1, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            /* Receive PDUSessionResourceSetupRequest +
            * DL NAS transport +
            * PDU session establishment accept */
            recvbuf = testgnb_ngap_read(ngap1);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);
            ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_PDUSessionResourceSetup,
            test_ue[i]->ngap_procedure_code);
           // printf("44444444444  NGAP_ProcedureCode_id_PDUSessionResourceSetup \n");

            /* Send GTP-U ICMP Packet */
            qos_flow = test_qos_flow_find_by_qfi(sess, 1);
            ogs_assert(qos_flow);
            rv = test_gtpu_send_ping(gtpu1, qos_flow, TEST_PING_IPV4);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            /* Send PDUSessionResourceSetupResponse */
            sendbuf = testngap_sess_build_pdu_session_resource_setup_response(sess);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap1, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);
           

            /* Receive GTP-U ICMP Packet */
            recvbuf = testgnb_gtpu_read(gtpu1);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            ogs_pkbuf_free(recvbuf);
            

            /* Send GTP-U ICMP Packet */
            rv = test_gtpu_send_ping(gtpu1, qos_flow, TEST_PING_IPV4);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            /* Receive GTP-U ICMP Packet */
            recvbuf = testgnb_gtpu_read(gtpu1);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            ogs_pkbuf_free(recvbuf);
         
         //   printf("44444444444  gtpu1 \n");

            /* Receive PDU session modification command */
            recvbuf = testgnb_ngap_read(ngap1);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);
          

            /* Send PDU session resource modify response */
            qos_flow = test_qos_flow_find_by_qfi(sess, 2);
            ogs_assert(qos_flow);

            sendbuf = testngap_build_qos_flow_resource_modify_response(qos_flow);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap1, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            /* Send PDU session resource modify complete */
            sess->ul_nas_transport_param.request_type =
            OGS_NAS_5GS_REQUEST_TYPE_MODIFICATION_REQUEST;
            sess->ul_nas_transport_param.dnn = 0;
            sess->ul_nas_transport_param.s_nssai = 0;

            sess->pdu_session_establishment_param.ssc_mode = 0;
            sess->pdu_session_establishment_param.epco = 0;
            //printf("44444444444  testngap_build_qos_flow_resource_modify_response \n");

            gsmbuf = testgsm_build_pdu_session_modification_complete(sess);
            ABTS_PTR_NOTNULL(tc, gsmbuf);
            gmmbuf = testgmm_build_ul_nas_transport(sess,
            OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION, gsmbuf);
            ABTS_PTR_NOTNULL(tc, gmmbuf);
            sendbuf = testngap_build_uplink_nas_transport(test_ue[i], gmmbuf);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap1, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            /* Waiting for creating dedicated QoS flow in PFCP protocol */
            ogs_msleep(100);

            /* Send GTP-U ICMP Packet */
            rv = test_gtpu_send_ping(gtpu1, qos_flow, TEST_PING_IPV4);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            /* Receive GTP-U ICMP Packet */
            recvbuf = testgnb_gtpu_read(gtpu1);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            ogs_pkbuf_free(recvbuf);
       // printf("44444444444  Receive GTP-U ICMP Packet \n");
#if 0
            /* Send Path Switch Request */
            test_ue[i]->nr_cgi.cell_id = 0x40002;
            test_ue[i]->ran_ue_ngap_id++;
            sess->gnb_n3_addr = test_self()->gnb2_addr;
            
            printf("1111111111\n");
            

            sendbuf = testngap_build_path_switch_request(test_ue[i]);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap2, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            /* Receive End Mark */

            recvbuf = test_gtpu_read(gtpu1);

            ABTS_PTR_NOTNULL(tc, recvbuf);
            ogs_pkbuf_free(recvbuf);
            printf("Receive End Mark\n");

            /* Receive End Mark */
            recvbuf = test_gtpu_read(gtpu1);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            ogs_pkbuf_free(recvbuf);
            //printf("222333333333\n");

            /* Receive Path Switch Ack */
            recvbuf = testgnb_ngap_read(ngap2);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);

            /* Send GTP-U ICMP Packet */
            qos_flow = test_qos_flow_find_by_qfi(sess, 1);
            ogs_assert(qos_flow);
            rv = test_gtpu_send_ping(gtpu2, qos_flow, TEST_PING_IPV4);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            /* Receive GTP-U ICMP Packet */
            recvbuf = testgnb_gtpu_read(gtpu2);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            ogs_pkbuf_free(recvbuf);

            /* Send GTP-U ICMP Packet */
            qos_flow = test_qos_flow_find_by_qfi(sess, 2);
            ogs_assert(qos_flow);
            rv = test_gtpu_send_ping(gtpu2, qos_flow, TEST_PING_IPV4);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            /* Receive GTP-U ICMP Packet */
            recvbuf = testgnb_gtpu_read(gtpu2);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            ogs_pkbuf_free(recvbuf);

            /* Send Path Switch Request */
            test_ue[i]->nr_cgi.cell_id = 0x40001;
            test_ue[i]->ran_ue_ngap_id++;
            sess->gnb_n3_addr = test_self()->gnb1_addr;

            sendbuf = testngap_build_path_switch_request(test_ue[i]);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap1, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            /* Receive End Mark */
            recvbuf = test_gtpu_read(gtpu2);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            ogs_pkbuf_free(recvbuf);

            /* Receive End Mark */
            recvbuf = test_gtpu_read(gtpu2);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            ogs_pkbuf_free(recvbuf);

            /* Receive Path Switch Ack */
            recvbuf = testgnb_ngap_read(ngap1);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);

            /* Send GTP-U ICMP Packet */
            qos_flow = test_qos_flow_find_by_qfi(sess, 1);
            ogs_assert(qos_flow);
            rv = test_gtpu_send_ping(gtpu1, qos_flow, TEST_PING_IPV4);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            /* Receive GTP-U ICMP Packet */
            recvbuf = testgnb_gtpu_read(gtpu1);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            ogs_pkbuf_free(recvbuf);

            /* Send GTP-U ICMP Packet */
            qos_flow = test_qos_flow_find_by_qfi(sess, 2);
            ogs_assert(qos_flow);
            rv = test_gtpu_send_ping(gtpu1, qos_flow, TEST_PING_IPV4);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            /* Receive GTP-U ICMP Packet */
            recvbuf = testgnb_gtpu_read(gtpu1);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            ogs_pkbuf_free(recvbuf);
#endif
            /* Send UEContextReleaseRequest */
            sendbuf = testngap_build_ue_context_release_request(test_ue[i],
            NGAP_Cause_PR_radioNetwork, NGAP_CauseRadioNetwork_user_inactivity,
            true);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap1, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            /* Receive UEContextReleaseCommand */
            recvbuf = testgnb_ngap_read(ngap1);
            ABTS_PTR_NOTNULL(tc, recvbuf);
            testngap_recv(test_ue[i], recvbuf);
            ABTS_INT_EQUAL(tc,
            NGAP_ProcedureCode_id_UEContextRelease,
            test_ue[i]->ngap_procedure_code);

            /* Send UEContextReleaseComplete */
            sendbuf = testngap_build_ue_context_release_complete(test_ue[i]);
            ABTS_PTR_NOTNULL(tc, sendbuf);
            rv = testgnb_ngap_send(ngap1, sendbuf);
            ABTS_INT_EQUAL(tc, OGS_OK, rv);

            ogs_msleep(300);
        }
      test_context_final_ex();
      printf("test4  end \n");
    }  

abts_suite *test_multi_ue_multi_test2(abts_suite *suite)
{

    time_t curtime;
    time(&curtime);

    suite = ADD_SUITE(suite);
    int i;
    for(i=0;i<g_testcycleNum;i++){
        printf("\r\n");
        time(&curtime);
        printf(">>>>>>>>>>>>>>>>>g_testcycleNum:%d:%d Time:%s\r\n",g_testcycleNum,i,ctime(&curtime));
        abts_run_test(suite, muti_ue_threads, NULL);
        }
    return suite;
}
