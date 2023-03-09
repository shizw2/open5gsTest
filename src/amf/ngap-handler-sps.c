
#include "ngap-path.h"
#include "sbi-path.h"
#include "nas-path.h"
#include "ngap-handler-sps.h"
#include "ngap-handler.h"



int sps_handle_rev_ini_ngap(amf_internel_msg_header_t *pmsg,ogs_pkbuf_t *pkbuf)
{ 
		uint8_t *buf;
		int rev;
		ran_ue_t * ran_ue=NULL;
		
		ogs_assert(pmsg);
		
		ogs_assert(pkbuf);
		ogs_ngap_message_t message;
		ogs_pkbuf_t *pkbuftmp = NULL;
		int rc;
        ogs_info("SPS rev INTERNEL_MSG_NGAP !!!!1111111");
		print_buf(pkbuf->data,pkbuf->len);		   
        //amf_internel_msgbuf_t *pmsg_buf = (amf_internel_msgbuf_t *)pkbuf->data;	    
		buf=(uint8_t *)malloc(4096); 		
        NGAP_icps_send_head_t *pmsg_buf_head=(NGAP_icps_send_head_t *)malloc(sizeof(NGAP_icps_send_head_t));
                
		memcpy(pmsg_buf_head,pkbuf->data+sizeof(amf_internel_msg_header_t),sizeof(NGAP_icps_send_head_t)); 
        ogs_info("SPS rev INTERNEL_MSG_NGAP !!!!pmsg_buf_head->size：%lu",pmsg_buf_head->size);
		ogs_info("SPS rev INTERNEL_MSG_NGAP !!!!222222222ProcedureCode=== %lu",pmsg_buf_head->ProcedureCode);
        //ogs_info("SPS rev INTERNEL_MSG_NGAP !!!!pmsg_buf->msg_head.len:%lu,sizeof(pmsg_buf_head):%lu,lll: %lu",pmsg_buf->msg_head.len,sizeof(pmsg_buf_head),sizeof(pmsg_buf_head)+sizeof(pmsg_buf->msg_head));
	    //ogs_info("SPS rev INTERNEL_MSG_NGAP :%02x%02x%02x%02x==buuff==%02x%02x%02x%02x===",*(buf),*(buf+1),*(buf+2),*(buf+3),*(buf+4),*(buf+5),*(buf+6),*(buf+7));	
		if((pmsg_buf_head->ProcedureCode!=NGAP_ProcedureCode_id_InitialUEMessage)&&
			(pmsg_buf_head->ProcedureCode!=NGAP_ProcedureCode_id_UplinkNASTransport)&&
			(pmsg_buf_head->ProcedureCode!=NGAP_ProcedureCode_id_InitialContextSetup)&&
			(pmsg_buf_head->ProcedureCode!=NGAP_ProcedureCode_id_UERadioCapabilityInfoIndication)&&
			(pmsg_buf_head->ProcedureCode!=NGAP_ProcedureCode_id_UEContextReleaseRequest)){
			ogs_info("SPS rev INTERNEL_MSG_NGAP !!! Begin ogs_ngap_decode!!=== %lu",pmsg_buf_head->ProcedureCode);
			pkbuftmp=ogs_pkbuf_alloc(NULL, OGS_MAX_SDU_LEN);
			pkbuftmp->len=pmsg_buf_head->size;
			//pkbuftmp->data=buf;
			memcpy(pkbuftmp->data,pkbuf->data+sizeof(amf_internel_msg_header_t)+sizeof(NGAP_icps_send_head_t),pmsg_buf_head->size);
			rc = ogs_ngap_decode(&message, pkbuftmp);
			if(rc!=OGS_OK){
					ogs_error("SPS Cannot decode NGAP message");
					return OGS_ERROR;
				}			
			
	    }else{
				memcpy(buf,pkbuf->data+sizeof(amf_internel_msg_header_t)+sizeof(NGAP_icps_send_head_t),pmsg_buf_head->size);             
            }
			
        switch(pmsg_buf_head->ProcedureCode) { 
			case NGAP_ProcedureCode_id_InitialUEMessage:
				ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_InitialUEMessage");
                ran_ue=ran_ue_find_by_amf_ue_ngap_id(pmsg->amf_ue_ngap_id);
				if(!ran_ue)
					ran_ue=ran_ue_add_sps(pmsg->ran_ue_ngap_id,pmsg->amf_ue_ngap_id);
				if(ran_ue){
                   
					ran_ue->saved.nr_tai=pmsg->nr_tai;
			    	ran_ue->saved.nr_cgi=pmsg->nr_cgi;	
				   ogs_info("SPS pmsg_buf_head->UEContextRequest  = %ld ",pmsg_buf_head->UEContextRequest);
					if(pmsg_buf_head->UEContextRequest==NGAP_UEContextRequest_requested){
						ran_ue->ue_context_requested=true;
						ogs_info("SPS ran_ue->ue_context_requested  = true ");
						}
					rev=ngap_send_to_nas_sps(ran_ue, pmsg_buf_head->ProcedureCode,pmsg_buf_head->size,buf);//pmsg_buf_code->buf
			    }
				break;
			case NGAP_ProcedureCode_id_UplinkNASTransport:	         
                ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_UplinkNASTransport amf_ue_ngap_id=%lu",pmsg->amf_ue_ngap_id);
                ran_ue=ran_ue_find_by_amf_ue_ngap_id(pmsg->amf_ue_ngap_id);
				if(!ran_ue)
					ran_ue=ran_ue_add_sps(pmsg->ran_ue_ngap_id,pmsg->amf_ue_ngap_id);
				if(ran_ue){                   
					ran_ue->saved.nr_tai=pmsg->nr_tai;
			    	ran_ue->saved.nr_cgi=pmsg->nr_cgi;					   
					rev=ngap_send_to_nas_sps(ran_ue, pmsg_buf_head->ProcedureCode,pmsg_buf_head->size,buf);//pmsg_buf_code->buf
			    }
				break;
			case NGAP_ProcedureCode_id_InitialContextSetup:	
				ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_InitialContextSetup");
				ogs_info(" NGAP_ProcedureCode_id_InitialContextSetup-pmsg_buf_head->PDUsessioncount:%u",pmsg_buf_head->PDUsessioncount);
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
			case NGAP_ProcedureCode_id_UEContextReleaseRequest:
				ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_UEContextReleaseRequest");
				ogs_info(" NGAP_ProcedureCode_id_UEContextReleaseRequest-pmsg_buf_head->PDUsessioncount:%u",pmsg_buf_head->PDUsessioncount);
				ran_ue=ran_ue_find_by_amf_ue_ngap_id(pmsg->amf_ue_ngap_id);
			    if(ran_ue){
					ogs_info("find ue.");
					ngap_handle_ue_context_release_request_sps(ran_ue,pmsg_buf_head->PDUsessioncount,pmsg_buf_head->size,buf);
				}
				break;
			case NGAP_ProcedureCode_id_PathSwitchRequest:
				ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_PathSwitchRequest");
				break;
			case NGAP_ProcedureCode_id_HandoverPreparation:
				ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_HandoverPreparation");
				break;
			case NGAP_ProcedureCode_id_UplinkRANStatusTransfer:
				ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_UplinkRANStatusTransfer");
				break;
			case NGAP_ProcedureCode_id_HandoverNotification:
				ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_HandoverNotification");
				break;
			case NGAP_ProcedureCode_id_HandoverCancel:
				ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_HandoverCancel");
				break;
			case NGAP_ProcedureCode_id_ErrorIndication:
				ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_ErrorIndication");
				break;
			case NGAP_ProcedureCode_id_PDUSessionResourceSetup:				
				ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_PDUSessionResourceSetup");
				ran_ue=ran_ue_find_by_amf_ue_ngap_id(pmsg->amf_ue_ngap_id);
			    if(ran_ue)
					ngap_handle_pdu_session_resource_setup_response_sps(ran_ue,&message);
				break;
			case NGAP_ProcedureCode_id_PDUSessionResourceModify:
				ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_PDUSessionResourceModify");
				break;
			case NGAP_ProcedureCode_id_PDUSessionResourceRelease:
				ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_PDUSessionResourceRelease");
                ran_ue=ran_ue_find_by_amf_ue_ngap_id(pmsg->amf_ue_ngap_id);
			    if(ran_ue)
                    ngap_handle_pdu_session_resource_release_response_sps(ran_ue,&message);
				break;
			case NGAP_ProcedureCode_id_UEContextRelease:
                ran_ue=ran_ue_find_by_amf_ue_ngap_id(pmsg->amf_ue_ngap_id);
			    if(ran_ue)
                    ngap_handle_ue_context_release_complete_sps(ran_ue,&message);
				ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_UEContextRelease");
				break;
			case NGAP_ProcedureCode_id_HandoverResourceAllocation:
				ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_HandoverResourceAllocation");
				break;			
			
			default:
				ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP pmsg_buf_head->ProcedureCode=%lu",pmsg_buf_head->ProcedureCode);
			    break;
        }
        free (buf);
        free(pmsg_buf_head);
		ogs_ngap_free(&message);
		ogs_pkbuf_free(pkbuftmp);
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
		UERadioCapability=(NGAP_UERadioCapability_t *)malloc(sizeof(NGAP_UERadioCapability_t));
		UERadioCapability->buf=(uint8_t *)malloc(UERadioCa->size);
		UERadioCapability->size=UERadioCa->size;		
		memcpy(UERadioCapability->buf,buf+sizeof(size_t),UERadioCa->size);
		ogs_info("UERadioCapability->size:%lu sizeof(size_t)=%lu",UERadioCapability->size,sizeof(size_t));
		ogs_info("UERadioCapability->buf::%2x%2x%2x%2x ",*(UERadioCapability->buf),*(UERadioCapability->buf+1),*(UERadioCapability->buf+2),*(UERadioCapability->buf+3));
		if (ran_ue->amf_ue)
        	OGS_ASN_STORE_DATA(&ran_ue->amf_ue->ueRadioCapability,
                UERadioCapability);
    }
	if(UERadioCapability)
		free(UERadioCapability);
	
	return;
}


void ngap_handle_initial_context_setup_response_sps(ran_ue_t * ran_ue,uint8_t count,size_t size,uint8_t *buf)
{
		int i;
	
		amf_ue_t *amf_ue = NULL;
		
		amf_sess_t *sess = NULL;
		//uint64_t amf_ue_ngap_id;
		amf_nsmf_pdusession_sm_context_param_t param;
	
		
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

		if(PDUSessionItem)free(PDUSessionItem);
	
}
void ngap_handle_ue_context_release_request_sps        (ran_ue_t * ran_ue,uint8_t count,size_t size,uint8_t *buf)
{

	int i,j;
	if(ran_ue == NULL)
		return;
	if(buf  == NULL)
		return;

    amf_ue_t *amf_ue = NULL;
    amf_sess_t *sess = NULL;

   // NGAP_PDUSessionResourceListCxtRelReq_t *PDUSessionList = NULL;
    NGAP_PDUSessionResourceItemCxtRelReq_t *PDUSessionItem = NULL;    
	NGAP_Cause_t *Cause = NULL;
	NGAP_Cause_sps_t *Trancause=NULL;

	PDUSessionItem=malloc(sizeof(NGAP_PDUSessionResourceSetupItemCxtRes_t));
	Cause=malloc(sizeof(NGAP_Cause_t));
	
	if(PDUSessionItem && size){
		ogs_info("ngap_handle_ue_context_release_request_sps proc.");
		for(j=0;j<count;j++){
				memcpy(&(PDUSessionItem->pDUSessionID),(buf+j*sizeof(NGAP_PDUSessionID_t)),sizeof(NGAP_PDUSessionID_t));
			}
		//memcpy(Trancause,(buf+count*sizeof(NGAP_PDUSessionID_t)),sizeof(NGAP_Cause_sps_t));
		Trancause = (NGAP_Cause_sps_t*)(buf+count*sizeof(NGAP_PDUSessionID_t));
		Cause->present=Trancause->present;
		Cause->choice.radioNetwork=Trancause->choice.radioNetwork;
		}else{
			 ogs_error("No PDUSessionResourceSetupItemSURes");
                    ogs_assert(OGS_OK ==
                        ngap_send_error_indication2(
                            amf_ue, NGAP_Cause_PR_protocol,
                            NGAP_CauseProtocol_semantic_error));
                  
			return;
			}

    ogs_info("    RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld]",
            ran_ue->ran_ue_ngap_id, (long long)ran_ue->amf_ue_ngap_id);
    amf_ue = ran_ue->amf_ue;
    if (!amf_ue) {
        ogs_error("Cannot find AMF-UE Context [%lld]",
                (long long)ran_ue->amf_ue_ngap_id);
        ogs_assert(OGS_OK ==
            ngap_send_ran_ue_context_release_command(ran_ue,
                Cause->present, (int)Cause->choice.radioNetwork,
                NGAP_UE_CTX_REL_NG_CONTEXT_REMOVE, 0));
    } else {
        int xact_count = amf_sess_xact_count(amf_ue);

        amf_ue->deactivation.group = Cause->present;
        amf_ue->deactivation.cause = (int)Cause->choice.radioNetwork;

        if (count==0) {
			ogs_info("amf_sbi_send_deactivate_all_sessions.");
            amf_sbi_send_deactivate_all_sessions(
                    amf_ue, AMF_UPDATE_SM_CONTEXT_DEACTIVATED,
                    Cause->present, (int)Cause->choice.radioNetwork);
        } else {
            for (i = 0; i < count; i++) { 
               
                if (PDUSessionItem->pDUSessionID ==
                        OGS_NAS_PDU_SESSION_IDENTITY_UNASSIGNED) {
                    ogs_error("PDU Session Identity is unassigned");
                    ogs_assert(OGS_OK ==
                        ngap_send_error_indication2(
                            amf_ue, NGAP_Cause_PR_protocol,
                            NGAP_CauseProtocol_semantic_error));
                    return;
                }

                sess = amf_sess_find_by_psi(amf_ue,
                        PDUSessionItem->pDUSessionID);
                if (SESSION_CONTEXT_IN_SMF(sess)) {
					ogs_info("amf_sbi_send_deactivate_session.");
                    amf_sbi_send_deactivate_session(
                            sess, AMF_UPDATE_SM_CONTEXT_DEACTIVATED,
                            Cause->present, (int)Cause->choice.radioNetwork);
                }else{
					ogs_info("SESSION_CONTEXT_IN_SMF not.");
				}
            }
        }

        if (amf_sess_xact_count(amf_ue) == xact_count)
            ogs_assert(OGS_OK ==
                ngap_send_amf_ue_context_release_command(amf_ue,
                    Cause->present, (int)Cause->choice.radioNetwork,
                    NGAP_UE_CTX_REL_NG_REMOVE_AND_UNLINK, 0));
    }

	return;
}

void ngap_handle_pdu_session_resource_setup_response_sps(ran_ue_t * ran_ue,ogs_ngap_message_t *message)
{
	char buf[OGS_ADDRSTRLEN];
	int i;
	
	amf_ue_t *amf_ue = NULL;
	
	//uint64_t amf_ue_ngap_id;
	amf_nsmf_pdusession_sm_context_param_t param;
	
	NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
	NGAP_PDUSessionResourceSetupResponse_t *PDUSessionResourceSetupResponse;
	
	NGAP_PDUSessionResourceSetupResponseIEs_t *ie = NULL;
	NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
	NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
	NGAP_PDUSessionResourceSetupListSURes_t *PDUSessionList = NULL;
	NGAP_PDUSessionResourceSetupItemSURes_t *PDUSessionItem = NULL;
	NGAP_PDUSessionResourceFailedToSetupListSURes_t
			*PDUSessionFailedList = NULL;
	NGAP_PDUSessionResourceFailedToSetupItemSURes_t
			*PDUSessionFailedItem = NULL;
	OCTET_STRING_t *transfer = NULL;	
	
	successfulOutcome = message->choice.successfulOutcome;
	ogs_assert(successfulOutcome);
	PDUSessionResourceSetupResponse =
			&successfulOutcome->value.choice.PDUSessionResourceSetupResponse;
	ogs_assert(PDUSessionResourceSetupResponse);
	
	ogs_debug("PDUSessionResourceSetupResponse");
	
		for (i = 0; i < PDUSessionResourceSetupResponse->protocolIEs.list.count;
				i++) {
			ie = PDUSessionResourceSetupResponse->protocolIEs.list.array[i];
			switch (ie->id) {
			case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
				RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
				break;
			case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
				AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
				break;
			case NGAP_ProtocolIE_ID_id_PDUSessionResourceSetupListSURes:
				PDUSessionList =
					&ie->value.choice.PDUSessionResourceSetupListSURes;
				break;
			case NGAP_ProtocolIE_ID_id_PDUSessionResourceFailedToSetupListSURes:
				PDUSessionFailedList =
					&ie->value.choice.PDUSessionResourceFailedToSetupListSURes;
				break;
			default:
				break;
			}
		}	
		amf_ue = ran_ue->amf_ue;
		if (!amf_ue) {
			ogs_error("Cannot find AMF-UE Context [%lld]",
					(long long)ran_ue->amf_ue_ngap_id);

			ogs_assert(OGS_OK ==
				ngap_send_error_indication_sps(
					ran_ue,
					NGAP_Cause_PR_radioNetwork,
					NGAP_CauseRadioNetwork_unknown_local_UE_NGAP_ID));

			return;
		}
	
		if (PDUSessionList) {
			for (i = 0; i < PDUSessionList->list.count; i++) {
				amf_sess_t *sess = NULL;
				PDUSessionItem = (NGAP_PDUSessionResourceSetupItemSURes_t *)
					PDUSessionList->list.array[i];
	
				if (!PDUSessionItem) {
					ogs_error("No PDUSessionResourceSetupItemSURes");

					ogs_assert(OGS_OK ==
						ngap_send_error_indication2_sps(
							amf_ue,
							NGAP_Cause_PR_protocol,
							NGAP_CauseProtocol_semantic_error));

					return;
				}
	
				transfer = &PDUSessionItem->pDUSessionResourceSetupResponseTransfer;
				if (!transfer) {
					ogs_error("No PDUSessionResourceSetupResponseTransfer");


					ogs_assert(OGS_OK ==
						ngap_send_error_indication2_sps(
							amf_ue,
							NGAP_Cause_PR_protocol,
							NGAP_CauseProtocol_semantic_error));


					return;
				}
	
				if (PDUSessionItem->pDUSessionID ==
						OGS_NAS_PDU_SESSION_IDENTITY_UNASSIGNED) {
					ogs_error("PDU Session Identity is unassigned");

					ogs_assert(OGS_OK ==
						ngap_send_error_indication2_sps(
							amf_ue,
							NGAP_Cause_PR_protocol,
							NGAP_CauseProtocol_semantic_error));

					return;
				}
	
				sess = amf_sess_find_by_psi(amf_ue, PDUSessionItem->pDUSessionID);
				if (!sess) {
					ogs_error("Cannot find PDU Session ID [%d]",
							(int)PDUSessionItem->pDUSessionID);

					ogs_assert(OGS_OK ==
						ngap_send_error_indication2_sps(amf_ue,
							NGAP_Cause_PR_radioNetwork,
							NGAP_CauseRadioNetwork_unknown_PDU_session_ID));

					return;
				}
	
				if (!SESSION_CONTEXT_IN_SMF(sess)) {
					ogs_error("Session Context is not in SMF [%d]",
							(int)PDUSessionItem->pDUSessionID);


					ogs_assert(OGS_OK ==
						ngap_send_error_indication2_sps(amf_ue,
							NGAP_Cause_PR_radioNetwork,
							NGAP_CauseRadioNetwork_unknown_PDU_session_ID));


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
		} else if (PDUSessionFailedList) {
			for (i = 0; i < PDUSessionFailedList->list.count; i++) {
				amf_sess_t *sess = NULL;
				PDUSessionFailedItem =
					(NGAP_PDUSessionResourceFailedToSetupItemSURes_t *)
					PDUSessionFailedList->list.array[i];
	
				if (!PDUSessionFailedItem) {
					ogs_error("No PDUSessionResourceFailedToSetupItemSURes");
					ogs_assert(OGS_OK ==
						ngap_send_error_indication2_sps(
							amf_ue,
							NGAP_Cause_PR_protocol,
							NGAP_CauseProtocol_semantic_error));
					return;
				}
	
				transfer =
					&PDUSessionFailedItem->
						pDUSessionResourceSetupUnsuccessfulTransfer;
				if (!transfer) {
					ogs_error("No PDUSessionResourceSetupUnsuccessfulTransfer");
					ogs_assert(OGS_OK ==
						ngap_send_error_indication2_sps(
							amf_ue,
							NGAP_Cause_PR_protocol,
							NGAP_CauseProtocol_semantic_error));
					return;
				}
	
				if (PDUSessionFailedItem->pDUSessionID ==
						OGS_NAS_PDU_SESSION_IDENTITY_UNASSIGNED) {
					ogs_error("PDU Session Identity is unassigned");
					ogs_assert(OGS_OK ==
						ngap_send_error_indication2_sps(
							amf_ue,
							NGAP_Cause_PR_protocol,
							NGAP_CauseProtocol_semantic_error));
					return;
				}
	
				sess = amf_sess_find_by_psi(
						amf_ue, PDUSessionFailedItem->pDUSessionID);
				if (!sess) {
					ogs_error("Cannot find PDU Session ID [%d]",
							(int)PDUSessionFailedItem->pDUSessionID);
					ogs_assert(OGS_OK ==
						ngap_send_error_indication2_sps(
							amf_ue,
							NGAP_Cause_PR_radioNetwork,
							NGAP_CauseRadioNetwork_unknown_PDU_session_ID));
					return;
				}
	
				if (!SESSION_CONTEXT_IN_SMF(sess)) {
					ogs_error("Session Context is not in SMF [%d]",
							(int)PDUSessionFailedItem->pDUSessionID);
					ogs_assert(OGS_OK ==
						ngap_send_error_indication2_sps(amf_ue,
							NGAP_Cause_PR_radioNetwork,
							NGAP_CauseRadioNetwork_unknown_PDU_session_ID));
					return;
				}
	
				/*
				 * TS23.502
				 * 4.2.3 Service Request procedures
				 * 4.2.3.2 UE Triggered Service Request
				 *
				 * 15. ...
				 * If a PDU Session is rejected by the serving NG-RAN
				 * with an indication that the PDU Session was rejected
				 * because User Plane Security Enforcement is not supported
				 * in the serving NG-RAN and the User Plane Enforcement Policy
				 * indicates "Required" as described in clause 5.10.3
				 * of TS 23.501 [2], the SMF shall trigger the release
				 * of this PDU Session.
				 *
				 * In all other cases of PDU Session rejection,
				 * the SMF can decide whether to release the PDU Session
				 * or to deactivate the UP connection of this PDU Session.
				 *
				 *
				 * TS29.502
				 *
				 * 5.2.2.3.2
				 * Activation and Deactivation of the User Plane connection
				 * of a PDU session
				 * 5.2.2.3.2.2
				 * Activation of User Plane connectivity of a PDU session
				 *
				 * 3. ...
				 * N2 SM information received from the 5G-AN
				 * (see PDU Session Resource Setup Unsuccessful Transfer IE
				 * in clause 9.3.4.16 of 3GPP TS 38.413 [9]),
				 * including the Cause of the failure, if resources failed
				 * to be established for the PDU session.
				 *
				 * Upon receipt of this request, the SMF shall:
				 * - consider that the activation of the User Plane connection
				 *	 has failed and set the upCnxState attribute to DEACTIVATED"
				 *	 otherwise.
				 */
				memset(&param, 0, sizeof(param));
				param.n2smbuf = ogs_pkbuf_alloc(NULL, OGS_MAX_SDU_LEN);
				ogs_assert(param.n2smbuf);
				param.n2SmInfoType = OpenAPI_n2_sm_info_type_PDU_RES_SETUP_FAIL;
				ogs_pkbuf_put_data(param.n2smbuf, transfer->buf, transfer->size);
	
				amf_ue->deactivation.group = NGAP_Cause_PR_nas;
				amf_ue->deactivation.cause = NGAP_CauseNas_normal_release;
	
				ogs_assert(true ==
					amf_sess_sbi_discover_and_send(
						OGS_SBI_SERVICE_TYPE_NSMF_PDUSESSION, NULL,
						amf_nsmf_pdusession_build_update_sm_context,
						sess, AMF_UPDATE_SM_CONTEXT_SETUP_FAIL, &param));
	
				ogs_pkbuf_free(param.n2smbuf);
			}
		} else {
			ogs_error("No PDUSessionResourceList");
			ogs_assert(OGS_OK ==
				ngap_send_error_indication2_sps(amf_ue,
					NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error));
		}
    
	return;
}
void print_buf(unsigned char *buf, int len){
    int i;
    for(i=0;i<len;i++)
        {
        printf("%02x ",buf[i]);
        if (i%8==7)
        {
        printf("\r\n");
        }
    }
    printf("\r\n");
    fflush(stdout);
}
void ngap_handle_pdu_session_resource_release_response_sps(
        ran_ue_t *ran_ue,ogs_ngap_message_t *message)
{
    char buf[OGS_ADDRSTRLEN];
    int i;

    amf_ue_t *amf_ue = NULL;
    //ran_ue_t *ran_ue = NULL;
    uint64_t amf_ue_ngap_id;
    amf_nsmf_pdusession_sm_context_param_t param;

    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    NGAP_PDUSessionResourceReleaseResponse_t
        *PDUSessionResourceReleaseResponse;

    NGAP_PDUSessionResourceReleaseResponseIEs_t *ie = NULL;
    NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
    NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
    NGAP_PDUSessionResourceReleasedListRelRes_t *PDUSessionList = NULL;
    NGAP_PDUSessionResourceReleasedItemRelRes_t *PDUSessionItem = NULL;
    OCTET_STRING_t *transfer = NULL;

    //ogs_assert(gnb);
    //ogs_assert(gnb->sctp.sock);

    ogs_assert(message);
    successfulOutcome = message->choice.successfulOutcome;
    ogs_assert(successfulOutcome);
    PDUSessionResourceReleaseResponse =
        &successfulOutcome->value.choice.PDUSessionResourceReleaseResponse;
    ogs_assert(PDUSessionResourceReleaseResponse);

    ogs_debug("PDUSessionResourceReleaseResponse");

    for (i = 0; i < PDUSessionResourceReleaseResponse->protocolIEs.list.count;
            i++) {
        ie = PDUSessionResourceReleaseResponse->protocolIEs.list.array[i];
        switch (ie->id) {
        case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
            RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
            AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_PDUSessionResourceReleasedListRelRes:
            PDUSessionList =
                &ie->value.choice.PDUSessionResourceReleasedListRelRes;
            break;
        default:
            break;
        }
    }


    if (!AMF_UE_NGAP_ID) {
        ogs_error("No AMF_UE_NGAP_ID");
        ogs_assert(OGS_OK ==
            ngap_send_error_indication_sps(ran_ue, 
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error));
        return;
    }

    if (asn_INTEGER2ulong(AMF_UE_NGAP_ID,
                (unsigned long *)&amf_ue_ngap_id) != 0) {
        ogs_error("Invalid AMF_UE_NGAP_ID");
        ogs_assert(OGS_OK ==
            ngap_send_error_indication_sps(ran_ue, 
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error));
        return;
    }

    ran_ue = ran_ue_find_by_amf_ue_ngap_id(amf_ue_ngap_id);
    if (!ran_ue) {
        ogs_error("No RAN UE Context : AMF_UE_NGAP_ID[%lld]",
                (long long)amf_ue_ngap_id);
        ogs_assert(OGS_OK ==
            ngap_send_error_indication_sps(
                ran_ue,
                NGAP_Cause_PR_radioNetwork,
                NGAP_CauseRadioNetwork_unknown_local_UE_NGAP_ID));
        return;
    }

    ogs_debug("    RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld]",
            ran_ue->ran_ue_ngap_id, (long long)ran_ue->amf_ue_ngap_id);

    amf_ue = ran_ue->amf_ue;
    if (!amf_ue) {
        ogs_error("Cannot find AMF-UE Context [%lld]",
                (long long)amf_ue_ngap_id);
        ogs_assert(OGS_OK ==
            ngap_send_error_indication_sps(
                ran_ue,
                NGAP_Cause_PR_radioNetwork,
                NGAP_CauseRadioNetwork_unknown_local_UE_NGAP_ID));
        return;
    }

    if (!PDUSessionList) {
        ogs_error("No PDUSessionResourceReleasedListRelRes");
        ogs_assert(OGS_OK ==
            ngap_send_error_indication2_sps(amf_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error));
        return;
    }

    for (i = 0; i < PDUSessionList->list.count; i++) {
        amf_sess_t *sess = NULL;
        PDUSessionItem = (NGAP_PDUSessionResourceReleasedItemRelRes_t *)
            PDUSessionList->list.array[i];

        if (!PDUSessionItem) {
            ogs_error("No PDUSessionResourceReleasedItemRelRes");
            ogs_assert(OGS_OK ==
                ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error));
            return;
        }

        transfer = &PDUSessionItem->pDUSessionResourceReleaseResponseTransfer;
        if (!transfer) {
            ogs_error("No PDUSessionResourceReleaseResponseTransfer");
            ogs_assert(OGS_OK ==
                ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error));
            return;
        }

        if (PDUSessionItem->pDUSessionID ==
                OGS_NAS_PDU_SESSION_IDENTITY_UNASSIGNED) {
            ogs_error("PDU Session Identity is unassigned");
            ogs_assert(OGS_OK ==
                ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error));
            return;
        }

        sess = amf_sess_find_by_psi(amf_ue, PDUSessionItem->pDUSessionID);
        if (!sess) {
            ogs_error("Cannot find PDU Session ID [%d]",
                    (int)PDUSessionItem->pDUSessionID);
            ogs_assert(OGS_OK ==
                ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_radioNetwork,
                    NGAP_CauseRadioNetwork_unknown_PDU_session_ID));
            return;
        }

        if (!SESSION_CONTEXT_IN_SMF(sess)) {
            ogs_error("Session Context is not in SMF [%d]",
                    (int)PDUSessionItem->pDUSessionID);
            ogs_assert(OGS_OK ==
                ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_radioNetwork,
                    NGAP_CauseRadioNetwork_unknown_PDU_session_ID));
            return;
        }

        memset(&param, 0, sizeof(param));
        param.n2smbuf = ogs_pkbuf_alloc(NULL, OGS_MAX_SDU_LEN);
        ogs_assert(param.n2smbuf);
        param.n2SmInfoType = OpenAPI_n2_sm_info_type_PDU_RES_REL_RSP;
        ogs_pkbuf_put_data(param.n2smbuf, transfer->buf, transfer->size);

        ogs_assert(true ==
            amf_sess_sbi_discover_and_send(
                OGS_SBI_SERVICE_TYPE_NSMF_PDUSESSION, NULL,
                amf_nsmf_pdusession_build_update_sm_context,
                sess, AMF_UPDATE_SM_CONTEXT_N2_RELEASED, &param));

        ogs_pkbuf_free(param.n2smbuf);
    }
}
        
void ngap_handle_ue_context_release_complete_sps(
                ran_ue_t *ran_ue,ogs_ngap_message_t *message)
{
            int i;
            char buf[OGS_ADDRSTRLEN];
            uint64_t amf_ue_ngap_id;
        
            //ran_ue_t *ran_ue = NULL;
        
            NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
            NGAP_UEContextReleaseComplete_t *UEContextReleaseComplete = NULL;
        
            NGAP_UEContextReleaseComplete_IEs_t *ie = NULL;
            NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
            NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
          
        
            //ogs_assert(gnb);
           // ogs_assert(gnb->sctp.sock);
        
            ogs_assert(message);
            successfulOutcome = message->choice.successfulOutcome;
            ogs_assert(successfulOutcome);
            UEContextReleaseComplete =
                &successfulOutcome->value.choice.UEContextReleaseComplete;
            ogs_assert(UEContextReleaseComplete);
        
            ogs_debug("UEContextReleaseComplete");
        
            for (i = 0; i < UEContextReleaseComplete->protocolIEs.list.count; i++) {
                ie = UEContextReleaseComplete->protocolIEs.list.array[i];
                switch (ie->id) {
                case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                    RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
                    break;
                case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                    AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
                    break;
                default:
                    break;
                }
            }                
        
            if (!AMF_UE_NGAP_ID) {
                ogs_error("No AMF_UE_NGAP_ID");
                ogs_assert(OGS_OK ==
                    ngap_send_error_indication_sps(ran_ue,
                        NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error));
                return;
            }
        
            if (asn_INTEGER2ulong(AMF_UE_NGAP_ID,
                        (unsigned long *)&amf_ue_ngap_id) != 0) {
                ogs_error("Invalid AMF_UE_NGAP_ID");
                ogs_assert(OGS_OK ==
                    ngap_send_error_indication_sps(ran_ue,
                        NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error));
                return;
            }
        
            ran_ue = ran_ue_find_by_amf_ue_ngap_id(amf_ue_ngap_id);
            if (!ran_ue) {
                ogs_error("No RAN UE Context : AMF_UE_NGAP_ID[%lld]",
                        (long long)amf_ue_ngap_id);
                ogs_assert(OGS_OK ==
                    ngap_send_error_indication_sps(
                        ran_ue,
                        NGAP_Cause_PR_radioNetwork,
                        NGAP_CauseRadioNetwork_unknown_local_UE_NGAP_ID));
                return;
            }
          
            ngap_handle_ue_context_release_action(ran_ue);
           
           
}

