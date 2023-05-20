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

double __get_us(struct timeval t) {
	return (t.tv_sec * 1000000 + t.tv_usec);
}

extern int g_testNum;
extern int g_threadNum;

#define NUM_OF_TEST_UE 100

static ogs_thread_t *mthread;
 void muti_ue_func(void *data);

typedef struct threadinfo
{
    abts_case   *tc;
    int         clientIdx ;	
}T_threadinfo;

int iPthreadSize = 3;

test_ue_t *test_ues[MAX_THREAD][10000];

static void muti_ue_threads(abts_case *tc, void *data)
{
#if 1
    int iTmp;
    ogs_thread_t *id[MAX_THREAD];
    T_threadinfo threadInfo[MAX_THREAD];
	ogs_nas_5gs_mobile_identity_suci_t mobile_identity_suci;
    //test_ue_t *test_ue[MAX_THREAD][g_testNum];
	int i;
	bson_t *doc = NULL;
	test_sess_t *sess = NULL;
	
	iPthreadSize = g_threadNum;
	if  (g_threadNum > 100)
	{
		iPthreadSize = 100;
	}

    if (g_testNum > 1000){
        g_testNum = 1000;//每个线程最多1000个用户， 最多支持20个线程。因为scheme_output只有5位。高2位为线程号，低3位为序号
    }
	
	struct timeval start_time, stop_time;
    gettimeofday(&start_time, NULL);
	
	for (iTmp = 0; iTmp < iPthreadSize; iTmp++)
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

			imsi_index = iTmp*1000+ i + 1;
			//printf("imsi:%lu.\r\n",imsi_index);
			//ogs_uint64_to_buffer(imsi_index, 5, mobile_identity_suci.scheme_output);
			mobile_identity_suci.scheme_output[0] = imsi_index/10000%10;
			mobile_identity_suci.scheme_output[1] = imsi_index/1000%10;
			mobile_identity_suci.scheme_output[2] = imsi_index/100%10;
			mobile_identity_suci.scheme_output[3] = imsi_index/10%10;
			mobile_identity_suci.scheme_output[4] = imsi_index%10;

            
			test_ues[iTmp][i] = test_ue_add_by_suci(&mobile_identity_suci, 13);
            
			ogs_assert(test_ues[iTmp][i]);

			test_ues[iTmp][i]->nr_cgi.cell_id = 0x40001;

			test_ues[iTmp][i]->nas.registration.tsc = 0;
			test_ues[iTmp][i]->nas.registration.ksi = OGS_NAS_KSI_NO_KEY_IS_AVAILABLE;
			test_ues[iTmp][i]->nas.registration.follow_on_request = 1;
			test_ues[iTmp][i]->nas.registration.value = OGS_NAS_5GS_REGISTRATION_TYPE_INITIAL;

			test_ues[iTmp][i]->k_string = "465b5ce8b199b49faa5f0a2ee238a6bc";
			test_ues[iTmp][i]->opc_string = "e8ed289deba952e4283b54e88e6183ca";
		}


		//插入数据库单独统计
		for (i = 0; i < g_testNum; i++) {
#if 0
			/* Send PDU session establishment request */
			sess = test_sess_add_by_dnn_and_psi(test_ues[iTmp][i], "internet", 5);
			ogs_assert(sess);
#endif		
			/********** Insert Subscriber in Database */
			doc = test_db_new_simple(test_ues[iTmp][i]);
			ABTS_PTR_NOTNULL(tc, doc);
			ABTS_INT_EQUAL(tc, OGS_OK, test_db_insert_ue(test_ues[iTmp][i], doc));
		}
    }
	gettimeofday(&stop_time, NULL);
	printf("Insert Subscriber in Database,Time use %f ms\n",(__get_us(stop_time) - __get_us(start_time)) / 1000);

    gettimeofday(&start_time, NULL);
	for (iTmp = 0; iTmp < iPthreadSize; iTmp++)
	{        
        for (i = 0; i < g_testNum; i++) {

            /* Send PDU session establishment request */
            sess = test_sess_add_by_dnn_and_psi(test_ues[iTmp][i], "internet", 5);
            ogs_assert(sess);
        }
       
    }
    gettimeofday(&stop_time, NULL);
    printf("alloc session,Time use %f ms\n",(__get_us(stop_time) - __get_us(start_time)) / 1000);

    
    for (iTmp = 0; iTmp < iPthreadSize; iTmp++)
    {
        //T_threadinfo threadInfo;
        threadInfo[iTmp].tc = tc;
        threadInfo[iTmp].clientIdx = iTmp;
		printf("threadInfo.clientIdx=%u.\r\n",threadInfo[iTmp].clientIdx);
        id[iTmp] = ogs_thread_create(muti_ue_func, &threadInfo[iTmp]);
        if (!id[iTmp]) return ; 

        //ogs_msleep(5000);

    }
	
    for (iTmp = 0; iTmp < iPthreadSize; iTmp++)
    {
        ogs_thread_join_ex(id[iTmp]);
    }
        
    for (iTmp = 0; iTmp < iPthreadSize; iTmp++)
    {
        ogs_thread_destroy_ex(id[iTmp]); 
    }

    for (iTmp = 0; iTmp < iPthreadSize; iTmp++)
    {
		for (i = 0; i < g_testNum; i++) {
			/********** Remove Subscriber in Database */
			ABTS_INT_EQUAL(tc, OGS_OK, test_db_remove_ue(test_ues[iTmp][i]));
		}
	}

    /* Clear Test UE Context */
    test_ue_remove_all();
#endif

#if 0
    muti_ue_func(tc);
#endif
}

 void muti_ue_func(void *data)
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
    ogs_msleep(6000);//wait for app init
    struct timeval start_time, stop_time;
    gettimeofday(&start_time, NULL);

    ogs_nas_5gs_mobile_identity_suci_t mobile_identity_suci;
    //test_ue_t *test_ue[MAX_UE];
	test_ue_t **test_ue;
    test_sess_t *sess = NULL;
    test_bearer_t *qos_flow = NULL;

    bson_t *doc = NULL;

    T_threadinfo *threadInfo = (T_threadinfo *)data;
    
    abts_case *tc = threadInfo->tc;

    //printf("clientIdx:%d.\r\n",threadInfo->clientIdx);

    test_ue = &test_ues[threadInfo->clientIdx];

    test_thread_context_init(threadInfo->clientIdx);//thread
	
    /* gNB connects to AMF */
    //ngap = testngap_client(AF_INET);
    ngap =  testngap_client_n(AF_INET,threadInfo->clientIdx);
    ABTS_PTR_NOTNULL(tc, ngap);

    /* gNB connects to UPF */
    //gtpu = test_gtpu_server(1, AF_INET);
    //ABTS_PTR_NOTNULL(tc, gtpu);

    /* Send NG-Setup Reqeust */
    sendbuf = testngap_build_ng_setup_request(0x4000, 22);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive NG-Setup Response */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);

   
	//gettimeofday(&stop_time, NULL);
	//printf("Insert Subscriber in Database,Time use %f ms\n",(__get_us(stop_time) - __get_us(start_time)) / 1000);
	gettimeofday(&start_time, NULL);

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
        test_ue[i]->nr_tai.tac.v = i%2+1;
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

        /* Receive PDUSessionResourceSetupRequest +
         * DL NAS transport +
         * PDU session establishment accept */
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
    }

#if 1
    //gettimeofday(&start, NULL);
	gettimeofday(&stop_time, NULL);
	printf("Thread:%d, Registration,Time use %f ms\n",threadInfo->clientIdx,(__get_us(stop_time) - __get_us(start_time)) / 1000);
	gettimeofday(&start_time, NULL);

	
	for (i = 0; i < g_testNum; i++) {
		/* Send PDU session establishment request */
        sess = test_sess_find_by_psi(test_ue[i], 5);
        ogs_assert(sess);
		
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

        /* Send PDUSessionResourceSetupResponse */
        sendbuf = testngap_sess_build_pdu_session_resource_setup_response(sess);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
	}
#endif	
	gettimeofday(&stop_time, NULL);
	printf("Thread:%d,session establishment,Time use %f ms\n",threadInfo->clientIdx,(__get_us(stop_time) - __get_us(start_time)) / 1000);
	gettimeofday(&start_time, NULL);

    for (i = 0; i < g_testNum; i++) {
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


	gettimeofday(&stop_time, NULL);
	printf("Thread:%d,session release,Time use %f ms\n",threadInfo->clientIdx,(__get_us(stop_time) - __get_us(start_time)) / 1000);
	gettimeofday(&start_time, NULL);

    for (i = 0; i < g_testNum; i++) {
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
    }

	
	gettimeofday(&stop_time, NULL);
	printf("Thread:%d,D-register,Time use %f ms\n",threadInfo->clientIdx,(__get_us(stop_time) - __get_us(start_time)) / 1000);
	

    ogs_msleep(300);


    /* gNB disonncect from UPF */
    //testgnb_gtpu_close(gtpu);

    /* gNB disonncect from AMF */
    testgnb_ngap_close(ngap);

    /* Clear Test UE Context */
    //test_ue_remove_all();
    test_context_final_ex();
}

static void test1_func(abts_case *tc, void *data)
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
    ogs_msleep(2000);//wait for app init
    struct timeval start_time, stop_time;
    gettimeofday(&start_time, NULL);

    ogs_nas_5gs_mobile_identity_suci_t mobile_identity_suci;
    test_ue_t *test_ue[g_testNum];
    test_sess_t *sess = NULL;
    test_bearer_t *qos_flow = NULL;

    bson_t *doc = NULL;

    /* gNB connects to AMF */
    ngap = testngap_client(AF_INET);
    ABTS_PTR_NOTNULL(tc, ngap);

    /* gNB connects to UPF */
    gtpu = test_gtpu_server(1, AF_INET);
    ABTS_PTR_NOTNULL(tc, gtpu);

    /* Send NG-Setup Reqeust */
    sendbuf = testngap_build_ng_setup_request(0x4000, 22);
    ABTS_PTR_NOTNULL(tc, sendbuf);
    rv = testgnb_ngap_send(ngap, sendbuf);
    ABTS_INT_EQUAL(tc, OGS_OK, rv);

    /* Receive NG-Setup Response */
    recvbuf = testgnb_ngap_read(ngap);
    ABTS_PTR_NOTNULL(tc, recvbuf);

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

        imsi_index = i + 1;
        //ogs_uint64_to_buffer(imsi_index, 5, mobile_identity_suci.scheme_output);
        mobile_identity_suci.scheme_output[0] = imsi_index/10000%10;
        mobile_identity_suci.scheme_output[1] = imsi_index/1000%10;
        mobile_identity_suci.scheme_output[2] = imsi_index/100%10;
        mobile_identity_suci.scheme_output[3] = imsi_index/10%10;
        mobile_identity_suci.scheme_output[4] = imsi_index%10;

        test_ue[i] = test_ue_add_by_suci(&mobile_identity_suci, 13);
        ogs_assert(test_ue[i]);

        test_ue[i]->nr_cgi.cell_id = 0x40001;

        test_ue[i]->nas.registration.tsc = 0;
        test_ue[i]->nas.registration.ksi = OGS_NAS_KSI_NO_KEY_IS_AVAILABLE;
        test_ue[i]->nas.registration.follow_on_request = 1;
        test_ue[i]->nas.registration.value = OGS_NAS_5GS_REGISTRATION_TYPE_INITIAL;

        test_ue[i]->k_string = "465b5ce8b199b49faa5f0a2ee238a6bc";
        test_ue[i]->opc_string = "e8ed289deba952e4283b54e88e6183ca";
    }


	//插入数据库单独统计
    for (i = 0; i < g_testNum; i++) {

        /********** Insert Subscriber in Database */
        doc = test_db_new_simple(test_ue[i]);
        ABTS_PTR_NOTNULL(tc, doc);
        ABTS_INT_EQUAL(tc, OGS_OK, test_db_insert_ue(test_ue[i], doc));
	}

    //printf("\r\n");

	gettimeofday(&stop_time, NULL);
	printf("Insert Subscriber in Database,Time use %f ms\n",(__get_us(stop_time) - __get_us(start_time)) / 1000);
	gettimeofday(&start_time, NULL);

    for (i = 0; i < g_testNum; i++) {
        if (i > 0)
            test_ue[i]->ran_ue_ngap_id = test_ue[i-1]->ran_ue_ngap_id;
        else
            test_ue[i]->ran_ue_ngap_id = 0;

        /* Send PDU session establishment request */
        sess = test_sess_add_by_dnn_and_psi(test_ue[i], "internet", 5);
        ogs_assert(sess);

        /********** Insert Subscriber in Database */
        //doc = test_db_new_simple(test_ue[i]);
        //ABTS_PTR_NOTNULL(tc, doc);
        //ABTS_INT_EQUAL(tc, OGS_OK, test_db_insert_ue(test_ue[i], doc));

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

        /* Receive PDUSessionResourceSetupRequest +
         * DL NAS transport +
         * PDU session establishment accept */
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
    }

#if 1
    //gettimeofday(&start, NULL);
	gettimeofday(&stop_time, NULL);
	printf("Registration,Time use %f ms\n",(__get_us(stop_time) - __get_us(start_time)) / 1000);
	gettimeofday(&start_time, NULL);

	
	for (i = 0; i < g_testNum; i++) {
		/* Send PDU session establishment request */
        sess = test_sess_find_by_psi(test_ue[i], 5);
        ogs_assert(sess);
		
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

        /* Send PDUSessionResourceSetupResponse */
        sendbuf = testngap_sess_build_pdu_session_resource_setup_response(sess);
        ABTS_PTR_NOTNULL(tc, sendbuf);
        rv = testgnb_ngap_send(ngap, sendbuf);
        ABTS_INT_EQUAL(tc, OGS_OK, rv);
	}
#endif	
	gettimeofday(&stop_time, NULL);
	printf("session establishment,Time use %f ms\n",(__get_us(stop_time) - __get_us(start_time)) / 1000);
	gettimeofday(&start_time, NULL);

    for (i = 0; i < g_testNum; i++) {
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
    }


	gettimeofday(&stop_time, NULL);
	printf("session release,Time use %f ms\n",(__get_us(stop_time) - __get_us(start_time)) / 1000);
	gettimeofday(&start_time, NULL);

    for (i = 0; i < g_testNum; i++) {
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
    }

	
	gettimeofday(&stop_time, NULL);
	printf("D-register,Time use %f ms\n",(__get_us(stop_time) - __get_us(start_time)) / 1000);
	

    ogs_msleep(300);

	gettimeofday(&start_time, NULL);
    for (i = 0; i < g_testNum; i++) {
        /********** Remove Subscriber in Database */
        ABTS_INT_EQUAL(tc, OGS_OK, test_db_remove_ue(test_ue[i]));
    }

	gettimeofday(&stop_time, NULL);
	printf("Remove Subscriber in Database,Time use %f ms\n",(__get_us(stop_time) - __get_us(start_time)) / 1000);


    /* gNB disonncect from UPF */
    testgnb_gtpu_close(gtpu);

    /* gNB disonncect from AMF */
    testgnb_ngap_close(ngap);

    /* Clear Test UE Context */
    test_ue_remove_all();

}

abts_suite *test_multi_ue(abts_suite *suite)
{
	struct timeval start_time, stop_time;
	gettimeofday(&start_time, NULL);

    suite = ADD_SUITE(suite);

    abts_run_test(suite, muti_ue_threads, NULL);

	gettimeofday(&stop_time, NULL);
	printf("ue num per thread:%d,Time use %f ms\n",g_testNum, (__get_us(stop_time) - __get_us(start_time)) / 1000 - 300);

    return suite;
}
