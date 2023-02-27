
#include "ngap-path.h"
#include "sbi-path.h"
#include "nas-path.h"
#include "ngap-handler-sps.h"


int sps_handle_rev_ini_ngap(amf_internel_msg_header_t *pmsg,ogs_pkbuf_t *pkbuf)
{ 
		uint8_t *buf;
		int rev;
		ran_ue_t * ran_ue=NULL;
		
		ogs_assert(pmsg);
		
		ogs_assert(pkbuf);
        ogs_info("SPS rev INTERNEL_MSG_NGAP !!!!1111111");
				   
        amf_internel_msgbuf_t *pmsg_buf = (amf_internel_msgbuf_t *)pkbuf->data;
	    buf=(uint8_t *)(pmsg+sizeof(amf_internel_msg_header_t)+sizeof(NGAP_icps_send_head_t));
				   
		pmsg_buf->buf=(uint8_t*)pkbuf->data+sizeof(pmsg_buf->msg_head);
		//printf("===len:%lu==========pmsg_buf->msg_head amf ue id:%lu  pmsg->amf_ue_ngap_id:%lu\n",pmsg_buf->msg_head.len,pmsg_buf->msg_head.amf_ue_ngap_id,pmsg->amf_ue_ngap_id);
        NGAP_icps_send_head_t *pmsg_buf_head=(NGAP_icps_send_head_t *)pmsg_buf->buf;
	    memcpy(buf,pkbuf->data+sizeof(amf_internel_msg_header_t)+sizeof(NGAP_icps_send_head_t),pmsg_buf_head->size);
		ogs_info("SPS rev INTERNEL_MSG_NGAP !!!!pmsg_buf_head->size：%lu",pmsg_buf_head->size);		   
		ogs_info("SPS rev INTERNEL_MSG_NGAP !!!!222222222ProcedureCode=== %lu",pmsg_buf_head->ProcedureCode);
        ogs_info("SPS rev INTERNEL_MSG_NGAP !!!!pmsg_buf->msg_head.len:%lu,sizeof(pmsg_buf_head):%lu,lll: %lu",pmsg_buf->msg_head.len,sizeof(pmsg_buf_head),sizeof(pmsg_buf_head)+sizeof(pmsg_buf->msg_head));
	    ogs_info("SPS rev INTERNEL_MSG_NGAP :%02x%02x%02x%02x==buuff==%02x%02x%02x%02x===",*(buf),*(buf+1),*(buf+2),*(buf+3),*(buf+4),*(buf+5),*(buf+6),*(buf+7));		          
        switch(pmsg_buf_head->ProcedureCode) { 
			case NGAP_ProcedureCode_id_InitialUEMessage:
				ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_InitialUEMessage");
			case NGAP_ProcedureCode_id_UplinkNASTransport:	         
                ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_UplinkNASTransport amf_ue_ngap_id=%lu",pmsg->amf_ue_ngap_id);
                ran_ue=ran_ue_find_by_amf_ue_ngap_id(pmsg->amf_ue_ngap_id);
				if(!ran_ue)
					ran_ue=ran_ue_add_sps(pmsg_buf->msg_head.ran_ue_ngap_id,pmsg_buf->msg_head.amf_ue_ngap_id);
				if(ran_ue){
					ran_ue->saved.nr_tai=pmsg_buf->msg_head.nr_tai;
			    	ran_ue->saved.nr_cgi=pmsg_buf->msg_head.nr_cgi;								
					rev=ngap_send_to_nas_sps(ran_ue, pmsg_buf_head->ProcedureCode,pmsg_buf_head->size,buf);//pmsg_buf_code->buf
			    }
				break;
			case NGAP_ProcedureCode_id_InitialContextSetup:	
				ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_InitialContextSetup");
				ran_ue=ran_ue_find_by_amf_ue_ngap_id(pmsg->amf_ue_ngap_id);
			    if(ran_ue)
					ngap_handle_initial_context_setup_response_sps(ran_ue,pmsg_buf_head->PDUsessioncount,pmsg_buf_head->size,buf);
				break;
			case NGAP_ProcedureCode_id_UERadioCapabilityInfoIndication:
				ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_UERadioCapabilityInfoIndication");
				ran_ue=ran_ue_find_by_amf_ue_ngap_id(pmsg->amf_ue_ngap_id);
			    if(ran_ue)
					ngap_handle_ue_radio_capability_info_indication_sps(ran_ue,pmsg_buf_head->size,buf);
				break;
			default:
				ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP pmsg_buf_head->ProcedureCode=%lu",pmsg_buf_head->ProcedureCode);
			    ran_ue=ran_ue_find_by_amf_ue_ngap_id(pmsg->amf_ue_ngap_id);
				if(!ran_ue)
					ran_ue=ran_ue_add_sps(pmsg_buf->msg_head.ran_ue_ngap_id,pmsg_buf->msg_head.amf_ue_ngap_id);
				if(ran_ue){
				    ran_ue->saved.nr_tai=pmsg_buf->msg_head.nr_tai;
					ran_ue->saved.nr_cgi=pmsg_buf->msg_head.nr_cgi;							   
					rev=ngap_send_to_nas_sps(ran_ue, pmsg_buf_head->ProcedureCode,pmsg_buf_head->size,buf);//pmsg_buf_code->buf
				}

			break;
        }
		return rev;
}


void ngap_handle_ue_radio_capability_info_indication_sps(ran_ue_t * ran_ue,size_t size,uint8_t *buf)
{
	NGAP_UERadioCapability_t *UERadioCapability = NULL;
	UERadioCapability_t *UERadioCa = NULL;
	ogs_assert(ran_ue);
	ogs_assert(buf);
	UERadioCa=(UERadioCapability_t *)buf;
	ogs_info("UERadioCapability buf::%2x%2x%2x%2x ",*(buf+8),*(buf+9),*(buf+10),*(buf+11));
	
	ogs_info("ngap_handle_ue_radio_capability_info_indication_sps　size＝＝　%lu",size);
    if(size)	{
		UERadioCapability=malloc(sizeof(NGAP_UERadioCapability_t));
		UERadioCapability->size=UERadioCa->size;		
		memcpy(UERadioCapability->buf,buf+sizeof(size_t),UERadioCa->size);
		ogs_info("UERadioCapability->size:%lu sizeof(size_t)=%lu",UERadioCapability->size,sizeof(size_t));
		ogs_info("UERadioCapability->buf::%2x%2x%2x%2x ",*(UERadioCapability->buf),*(UERadioCapability->buf+1),*(UERadioCapability->buf+2),*(UERadioCapability->buf+3));
		if (ran_ue->amf_ue)
        	OGS_ASN_STORE_DATA(&ran_ue->amf_ue->ueRadioCapability,
                UERadioCapability);
    }
	//if(UERadioCapability)
		//free(UERadioCapability);
	return;
}


void ngap_handle_initial_context_setup_response_sps(ran_ue_t * ran_ue,uint8_t count,size_t size,uint8_t *buf)
{
		int i;
	
		amf_ue_t *amf_ue = NULL;
		
		amf_sess_t *sess = NULL;
		//uint64_t amf_ue_ngap_id;
		amf_nsmf_pdusession_sm_context_param_t param;
	
		//NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
		//NGAP_InitialContextSetupResponse_t *InitialContextSetupResponse = NULL;
	
		//NGAP_InitialContextSetupResponseIEs_t *ie = NULL;
		//NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
		//NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
		//NGAP_PDUSessionResourceSetupListCxtRes_t *PDUSessionList = NULL;
		NGAP_PDUSessionResourceSetupItemCxtRes_t *PDUSessionItem = NULL;
		OCTET_STRING_t *transfer = NULL;	

		PDUSessionResource_t *pPDUSess;

		PDUSessionItem=malloc(sizeof(NGAP_PDUSessionResourceSetupItemCxtRes_t));
        
		pPDUSess=(PDUSessionResource_t *)buf;    
		

		amf_ue = ran_ue->amf_ue;
		if (!amf_ue) {
			ogs_error("Cannot find AMF-UE Context [%lld]",
					(long long)ran_ue->amf_ue_ngap_id);
#if 0
			ogs_assert(OGS_OK ==
				ngap_send_error_indication(
					gnb, &ran_ue->ran_ue_ngap_id, &ran_ue->amf_ue_ngap_id,
					NGAP_Cause_PR_radioNetwork,
					NGAP_CauseRadioNetwork_unknown_local_UE_NGAP_ID));
#endif
			return;
		}

	
		 
	
		for (i = 0; i <count; i++) {
			pPDUSess->buf=buf+sizeof(pPDUSess->pDUSessionID)+pPDUSess->size;
			PDUSessionItem->pDUSessionID = pPDUSess->pDUSessionID;
			PDUSessionItem->pDUSessionResourceSetupResponseTransfer.size=pPDUSess->size;
			PDUSessionItem->pDUSessionResourceSetupResponseTransfer.buf=pPDUSess->buf;
            pPDUSess->buf=pPDUSess->buf+pPDUSess->size;
			if (!PDUSessionItem) {
				ogs_error("No PDUSessionResourceSetupItemCxtRes");
				#if 0
				ogs_assert(OGS_OK ==
					ngap_send_error_indication2(amf_ue,
						NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error));
				#endif
				return;
			}
	
			transfer = &PDUSessionItem->pDUSessionResourceSetupResponseTransfer;
			if (!transfer) {
				ogs_error("No PDUSessionResourceSetupResponseTransfer");
				#if 0
				ogs_assert(OGS_OK ==
					ngap_send_error_indication2(amf_ue,
						NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error));
				#endif
				return;
			}
	
			if (PDUSessionItem->pDUSessionID ==
					OGS_NAS_PDU_SESSION_IDENTITY_UNASSIGNED) {
				ogs_error("PDU Session Identity is unassigned");
				#if 0
				ogs_assert(OGS_OK ==
					ngap_send_error_indication2(amf_ue,
						NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error));
				#endif
				return;
			}
	
			sess = amf_sess_find_by_psi(amf_ue, PDUSessionItem->pDUSessionID);
			if (!sess) {
				ogs_error("Cannot find PDU Session ID [%d]",
						(int)PDUSessionItem->pDUSessionID);
				#if 0
				ogs_assert(OGS_OK ==
					ngap_send_error_indication2(amf_ue,
						NGAP_Cause_PR_radioNetwork,
						NGAP_CauseRadioNetwork_unknown_PDU_session_ID));
				#endif
				return;
			}
	
			if (!SESSION_CONTEXT_IN_SMF(sess)) {
				ogs_error("Session Context is not in SMF [%d]",
						(int)PDUSessionItem->pDUSessionID);
				#if 0
				ogs_assert(OGS_OK ==
					ngap_send_error_indication2(amf_ue,
						NGAP_Cause_PR_radioNetwork,
						NGAP_CauseRadioNetwork_unknown_PDU_session_ID));
				#endif
				return;
			}
	
			memset(&param, 0, sizeof(param));
			param.n2smbuf = ogs_pkbuf_alloc(NULL, OGS_MAX_SDU_LEN);
			ogs_assert(param.n2smbuf);
			param.n2SmInfoType = OpenAPI_n2_sm_info_type_PDU_RES_SETUP_RSP;
			ogs_pkbuf_put_data(param.n2smbuf, transfer->buf, transfer->size);
	
			ogs_assert(true ==
				amf_sess_sbi_discover_and_send(
					OGS_SBI_SERVICE_TYPE_NSMF_PDUSESSION, NULL,
					amf_nsmf_pdusession_build_update_sm_context,
					sess, AMF_UPDATE_SM_CONTEXT_ACTIVATED, &param));
	
			ogs_pkbuf_free(param.n2smbuf);

			
		}
		
#if 0
	
		/*
		 * TS24.501
		 * 5.4.4 Generic UE configuration update procedure
		 * 5.4.4.1 General
		 *
		 * This procedure shall be initiated by the network to assign
		 * a new 5G-GUTI to the UE after a successful service request
		 * procedure invoked as a response to a paging request
		 * from the network and before the release of the N1 NAS signalling
		 * connection.
		 */
		if (DOWNLINK_SIGNALLING_PENDING(amf_ue) == true) {
			/*
			 * TS24.501
			 * 5.4.4 Generic UE configuration update procedure
			 * 5.4.4.1 General
			 *
			 * If the service request procedure was triggered
			 * due to 5GSM downlink signalling pending, the procedure
			 * for assigning a new 5G-GUTI can be initiated by the network
			 * after the transport of the 5GSM downlink signalling.
			 */
			ogs_list_for_each(&amf_ue->sess_list, sess) {
				ogs_pkbuf_t *ngapbuf = NULL;
				ogs_pkbuf_t *gmmbuf = NULL;
	
				/* There is no Downlink Signalling Pending in this sesssion */
				if (sess->gsm_message.type == 0) continue;
	
				switch (sess->gsm_message.type) {
				case OGS_NAS_5GS_PDU_SESSION_MODIFICATION_COMMAND:
					gmmbuf = gmm_build_dl_nas_transport(sess,
							OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION,
							sess->gsm_message.n1buf, 0, 0);
					ogs_assert(gmmbuf);
	
					ngapbuf = ngap_build_pdu_session_resource_modify_request(
							sess, gmmbuf, sess->gsm_message.n2buf);
					ogs_assert(ngapbuf);
	
					if (nas_5gs_send_to_gnb(amf_ue, ngapbuf) != OGS_OK) {
						ogs_error("nas_5gs_send_to_gnb() failed");
					}
	
					/* n1buf is de-allocated
					 * in gmm_build_dl_nas_transport() */
					sess->gsm_message.n1buf = NULL;
					/* n2buf is de-allocated
					 * in ngap_build_pdu_session_resource_modify_request() */
					sess->gsm_message.n2buf = NULL;
	
					AMF_SESS_CLEAR_5GSM_MESSAGE(sess);
	
					break;
				default:
					ogs_fatal("Unknown GSM Message Type[%d]",
							sess->gsm_message.type);
					ogs_assert_if_reached();
				}
			}
		} else if (PAGING_ONGOING(amf_ue) == true) {
			gmm_configuration_update_command_param_t param;
	
			/*
			 * TS24.501
			 * 5.3.3 Temporary identities
			 *
			 * The AMF shall assign a new 5G-GUTI for a particular UE:
			 *
			 * a) during a successful initial registration procedure;
			 * b) during a successful registration procedure
			 *	  for mobility registration update; and
			 * c) after a successful service request procedure invoked
			 *	  as a response to a paging request from the network and
			 *	  before the release of the N1 NAS signalling connection
			 *	  as specified in subclause 5.4.4.1.
			 *
			 * The AMF should assign a new 5G-GUTI for a particular UE
			 * during a successful registration procedure
			 * for periodic registration update.
			 *
			 * The AMF may assign a new 5G-GUTI at any time for a particular UE
			 * by performing the generic UE configuration update procedure.
			 */
			amf_ue_new_guti(amf_ue);
	
			memset(&param, 0, sizeof(param));
			param.acknowledgement_requested = 1;
			param.guti = 1;
			ogs_assert(OGS_OK ==
				nas_5gs_send_configuration_update_command(amf_ue, &param));
	
			AMF_UE_CLEAR_PAGING_INFO(amf_ue);
		}
#endif 
		if(PDUSessionItem)free(PDUSessionItem);
	
}

