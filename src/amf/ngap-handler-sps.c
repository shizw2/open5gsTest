
#include "ngap-path.h"
#include "sbi-path.h"
#include "nas-path.h"
#include "ngap-handler-sps.h"
#include "ngap-handler.h"
#include "udp-ini-path.h"

extern int send_heart_cnt;

int sps_handle_rev_ini_ngap(amf_internel_msg_header_t *pmsg,ogs_pkbuf_t *pkbuf)
{ 
    uint8_t *buf=NULL;
    int rev = OGS_OK;
    ran_ue_t * ran_ue=NULL;
    amf_ue_t *amf_ue = NULL;
    ogs_assert(pmsg);
    
    ogs_assert(pkbuf);
    ogs_ngap_message_t message;
    memset(&message, 0, sizeof(ogs_ngap_message_t));
    ogs_pkbuf_t *pkbuftmp = NULL;
    int rc;
    ogs_debug("SPS rev INTERNEL_MSG_NGAP !!!!1111111pmsg->down_ngap_type=%d",pmsg->down_ngap_type);
    if((pmsg->down_ngap_type==AMF_REMOVE_S1_CONTEXT_BY_LO_CONNREFUSED )||
            (pmsg->down_ngap_type == AMF_REMOVE_S1_CONTEXT_BY_RESET_ALL)){
            buf=(uint8_t *)ogs_malloc(OGS_MAX_SDU_LEN);
            memcpy(buf,pkbuf->data+sizeof(amf_internel_msg_header_t),pmsg->len); 
            //print_buf(pkbuf->data,sizeof(amf_internel_msg_header_t)+pmsg->len);
            amf_sbi_send_deactivate_all_ue_in_gnb_sps(buf, pmsg->len,pmsg->down_ngap_type); 
            return OGS_OK;
            }           
    
    NGAP_icps_send_head_t *pmsg_buf_head = (NGAP_icps_send_head_t *)(pkbuf->data+sizeof(amf_internel_msg_header_t));    
    ogs_info("SPS rev INTERNEL_MSG_NGAP !!! Begin ogs_ngap_decode!!=== %lu",pmsg_buf_head->ProcedureCode);		 
    ogs_pkbuf_pull(pkbuf,sizeof(amf_internel_msg_header_t)+sizeof(NGAP_icps_send_head_t));
    
    rc = ogs_ngap_decode(&message, pkbuf);
    if(rc!=OGS_OK){
			ogs_error("SPS Cannot decode NGAP message");        
        return OGS_ERROR;
    }			

    switch(pmsg_buf_head->ProcedureCode) {  
                            
        case NGAP_ProcedureCode_id_InitialUEMessage:
            ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_InitialUEMessage");
            // ran_ue=ran_ue_find_by_amf_ue_ngap_id(pmsg->amf_ue_ngap_id);
            ran_ue=ran_ue_find_by_amf_ue_ngap_id_sps(&(pmsg->amf_ue_ngap_id));
            if(!ran_ue){
                ran_ue=ran_ue_add_sps(pmsg->ran_ue_ngap_id,pmsg->amf_ue_ngap_id);

            }
            if(ran_ue){                    			    
                ngap_handle_initial_ue_message_sps(ran_ue,&message);					
            }
            break;
        case NGAP_ProcedureCode_id_UplinkNASTransport:	         
            ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_UplinkNASTransport amf_ue_ngap_id=%lu",pmsg->amf_ue_ngap_id);
            ran_ue=ran_ue_find_by_amf_ue_ngap_id_sps(&(pmsg->amf_ue_ngap_id));
            if(!ran_ue){
                ran_ue=ran_ue_add_sps(pmsg->ran_ue_ngap_id,pmsg->amf_ue_ngap_id);
            }
            if(ran_ue){
                ngap_handle_uplink_nas_transport_sps(ran_ue,&message);					
            }
            break;
        case NGAP_ProcedureCode_id_InitialContextSetup:	
            ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_InitialContextSetup");
            //ogs_info(" NGAP_ProcedureCode_id_InitialContextSetup-pmsg_buf_head->PDUsessioncount:%u",pmsg_buf_head->PDUsessioncount);
            ran_ue=ran_ue_find_by_amf_ue_ngap_id_sps(&(pmsg->amf_ue_ngap_id));
            if(ran_ue)
                ngap_handle_initial_context_setup_response_sps(ran_ue,&message);
            break;
        case NGAP_ProcedureCode_id_UERadioCapabilityInfoIndication:				
            ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_UERadioCapabilityInfoIndication");
            
            ran_ue=ran_ue_find_by_amf_ue_ngap_id_sps(&(pmsg->amf_ue_ngap_id));
            if(ran_ue)
                ngap_handle_ue_radio_capability_info_indication_sps(ran_ue,&message);
            break;
        case NGAP_ProcedureCode_id_UEContextReleaseRequest:
            ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_UEContextReleaseRequest");
            ogs_info(" NGAP_ProcedureCode_id_UEContextReleaseRequest-pmsg_buf_head->PDUsessioncount:%u",pmsg_buf_head->PDUsessioncount);
            ran_ue=ran_ue_find_by_amf_ue_ngap_id_sps(&(pmsg->amf_ue_ngap_id));
            if(ran_ue)
                ngap_handle_ue_context_release_request_sps(ran_ue,&message);
            break;
        case NGAP_ProcedureCode_id_PathSwitchRequest:                
            ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_PathSwitchRequest");
            ran_ue=ran_ue_find_by_amf_ue_ngap_id_sps(&(pmsg->amf_ue_ngap_id));
            if(ran_ue){
                ran_ue->ran_ue_ngap_id=pmsg->ran_ue_ngap_id;
                ngap_handle_path_switch_request_sps(ran_ue,&message);
            }
            break;
        case NGAP_ProcedureCode_id_HandoverPreparation:
            ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_HandoverPreparation");
            ran_ue=ran_ue_find_by_amf_ue_ngap_id_sps(&(pmsg->amf_ue_ngap_id));
            if(ran_ue){
                ran_ue->ran_ue_ngap_id=pmsg->ran_ue_ngap_id;
                    ngap_handle_handover_required_sps(pmsg->pre_amf_ue_ngap_id,ran_ue,&message);
            }
            break;				
        case NGAP_ProcedureCode_id_UplinkRANStatusTransfer:
            ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_UplinkRANStatusTransfer");
            break;
        case NGAP_ProcedureCode_id_HandoverNotification:
            ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_HandoverNotification");
            ran_ue=ran_ue_find_by_amf_ue_ngap_id_sps(&(pmsg->amf_ue_ngap_id));
            if(ran_ue){
                ran_ue->ran_ue_ngap_id=pmsg->ran_ue_ngap_id;
                ngap_handle_handover_notification_sps(ran_ue,&message);
            }                
            break;
        case NGAP_ProcedureCode_id_HandoverCancel:
            ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_HandoverCancel");
            ran_ue=ran_ue_find_by_amf_ue_ngap_id_sps(&(pmsg->amf_ue_ngap_id));
            if(ran_ue){
                ran_ue->ran_ue_ngap_id=pmsg->ran_ue_ngap_id;
                ngap_handle_handover_cancel_sps(ran_ue,&message);
            }
            break;
        case NGAP_ProcedureCode_id_ErrorIndication:
            ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_ErrorIndication");
            break;
        case NGAP_ProcedureCode_id_PDUSessionResourceSetup:				
            ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_PDUSessionResourceSetup");
            ran_ue=ran_ue_find_by_amf_ue_ngap_id_sps(&(pmsg->amf_ue_ngap_id));
            if(ran_ue)
                ngap_handle_pdu_session_resource_setup_response_sps(ran_ue,&message);
            break;
        case NGAP_ProcedureCode_id_PDUSessionResourceModify:
            ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_PDUSessionResourceModify");
                ran_ue=ran_ue_find_by_amf_ue_ngap_id_sps(&(pmsg->amf_ue_ngap_id));
			    if(ran_ue)
                    ngap_handle_pdu_session_resource_modify_response_sps(ran_ue,&message);
            break;
        case NGAP_ProcedureCode_id_PDUSessionResourceRelease:
            ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_PDUSessionResourceRelease");
            ran_ue=ran_ue_find_by_amf_ue_ngap_id_sps(&(pmsg->amf_ue_ngap_id));
            if(ran_ue)
                ngap_handle_pdu_session_resource_release_response_sps(ran_ue,&message);
            break;
        case NGAP_ProcedureCode_id_UEContextRelease:
            ran_ue=ran_ue_find_by_amf_ue_ngap_id_sps(&(pmsg->amf_ue_ngap_id));
            if(ran_ue)
                ngap_handle_ue_context_release_complete_sps(ran_ue,&message);
            ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_UEContextRelease");
            break;
        case NGAP_ProcedureCode_id_HandoverResourceAllocation:
            ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_HandoverResourceAllocation");
            ran_ue=ran_ue_find_by_amf_ue_ngap_id_sps(&(pmsg->amf_ue_ngap_id));
            if(ran_ue)
                ngap_handle_handover_request_ack_sps(ran_ue,&message);
            break;							
        case NGAP_ProcedureCode_id_HandoverResourceAllocation_Fail:
            ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_Handover fail");
            ran_ue=ran_ue_find_by_amf_ue_ngap_id_sps(&(pmsg->amf_ue_ngap_id));
            if(ran_ue)
                ngap_handle_handover_failure_sps(ran_ue,&message);
            break;
        case NGAP_ProcedureCode_id_InitialContextSetup_Fail:
            ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP NGAP_ProcedureCode_id_Handover fail");
            ran_ue=ran_ue_find_by_amf_ue_ngap_id_sps(&(pmsg->amf_ue_ngap_id));
            if(ran_ue)
                ngap_handle_initial_context_setup_failure_sps(ran_ue,&message);
            break;
        case NGAP_ProcedureCode_id_NGReset:
            ogs_info(">>>>>>>>SPS rev NGAP_ProcedureCode_id_NGReset");
            ngap_handle_ng_reset_sps(&message);
            break;
        default:
            ogs_info(">>>>>>>>SPS rev INTERNEL_MSG_NGAP pmsg_buf_head->ProcedureCode=%lu",pmsg_buf_head->ProcedureCode);
            break;
    }

    //if(&message)  //the address of ‘message’ will always evaluate as ‘true’ [-Werror=address]
        ogs_ngap_free(&message);
    return rev;
}


void ngap_handle_initial_ue_message_sps(ran_ue_t *ran_ue,ogs_ngap_message_t *message)
{
    int i,r;
    char buf[OGS_ADDRSTRLEN];

    ran_ue_t *ran_ue_old = NULL;

    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_InitialUEMessage_t *InitialUEMessage = NULL;

    NGAP_InitialUEMessage_IEs_t *ie = NULL;
    NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
    NGAP_NAS_PDU_t *NAS_PDU = NULL;
    NGAP_UserLocationInformation_t *UserLocationInformation = NULL;
    NGAP_UserLocationInformationNR_t *UserLocationInformationNR = NULL;
    NGAP_FiveG_S_TMSI_t *FiveG_S_TMSI = NULL;
    NGAP_UEContextRequest_t *UEContextRequest = NULL;    

    ogs_assert(message);
    initiatingMessage = message->choice.initiatingMessage;
    ogs_assert(initiatingMessage);
    InitialUEMessage = &initiatingMessage->value.choice.InitialUEMessage;
    ogs_assert(InitialUEMessage);

    ogs_info("InitialUEMessage");

    for (i = 0; i < InitialUEMessage->protocolIEs.list.count; i++) {
        ie = InitialUEMessage->protocolIEs.list.array[i];
        switch (ie->id) {
        case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
            RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_NAS_PDU:
            NAS_PDU = &ie->value.choice.NAS_PDU;
            break;
        case NGAP_ProtocolIE_ID_id_UserLocationInformation:
            UserLocationInformation =
                &ie->value.choice.UserLocationInformation;
            break;
        case NGAP_ProtocolIE_ID_id_FiveG_S_TMSI:
            FiveG_S_TMSI = &ie->value.choice.FiveG_S_TMSI;
            break;
        case NGAP_ProtocolIE_ID_id_UEContextRequest:            
            UEContextRequest = &ie->value.choice.UEContextRequest;
            break;
        default:
            break;
        }
    }
    

    if (!RAN_UE_NGAP_ID) {
        ogs_error("No RAN_UE_NGAP_ID");
        
        return;
    }

    //ran_ue = ran_ue_find_by_ran_ue_ngap_id(gnb, *RAN_UE_NGAP_ID);
    if (ran_ue) {
     ogs_info("SPS ngap_handle_initial_ue_message AMF UE  NGAP ID =======   %lu  \n",ran_ue->amf_ue_ngap_id);
        /* Find AMF_UE if 5G-S_TMSI included */
        if (FiveG_S_TMSI) {
            ogs_nas_5gs_guti_t nas_guti;
            amf_ue_t *amf_ue = NULL;
            uint8_t region;
            uint16_t set;
            uint8_t pointer;
            uint32_t m_tmsi;
            ogs_info("FiveG_S_TMSI is TRUE");

            memset(&nas_guti, 0, sizeof(ogs_nas_5gs_guti_t));

            /* Use the first configured plmn_id and mme group id */
            ogs_nas_from_plmn_id(&nas_guti.nas_plmn_id,
                    &amf_self()->served_guami[0].plmn_id);
            region = amf_self()->served_guami[0].amf_id.region;

            /* Getting from 5G-S_TMSI */
            ogs_ngap_AMFSetID_to_uint16(&FiveG_S_TMSI->aMFSetID, &set);
            ogs_ngap_AMFPointer_to_uint8(&FiveG_S_TMSI->aMFPointer, &pointer);

            ogs_amf_id_build(&nas_guti.amf_id, region, set, pointer);

            /* size must be 4 */
            ogs_asn_OCTET_STRING_to_uint32(&FiveG_S_TMSI->fiveG_TMSI, &m_tmsi);
            nas_guti.m_tmsi = m_tmsi;

            amf_ue = amf_ue_find_by_guti(&nas_guti);
            if (!amf_ue) {
                ogs_info("Unknown UE by 5G-S_TMSI[AMF_ID:0x%x,M_TMSI:0x%x]",
                    ogs_amf_id_hexdump(&nas_guti.amf_id), nas_guti.m_tmsi);
				
            } else {
                ogs_info("[%s]    5G-S_TMSI[AMF_ID:0x%x,M_TMSI:0x%x]",
                        AMF_UE_HAVE_SUCI(amf_ue) ? amf_ue->suci : "Unknown ID",
                        ogs_amf_id_hexdump(&amf_ue->current.guti.amf_id),
                        amf_ue->current.guti.m_tmsi);
                if(amf_ue->ran_ue){
                        ogs_info("SPS amf_ue->ran_ue->amf_ue_ngap_id:%lu",amf_ue->ran_ue->amf_ue_ngap_id);
                        //ran_ue->amf_ue_ngap_id=amf_ue->ran_ue->amf_ue_ngap_id;//add
				        ran_ue_old=ran_ue_find_by_amf_ue_ngap_id_sps(&(amf_ue->ran_ue->amf_ue_ngap_id));
                        if(ran_ue_old){
                            ogs_info("SPS amf_ue_associate_ran_ue ran_ue_old ===  %d ran_ue_old->amf_ue_ngap_id= %lu ",ran_ue_old->ran_ue_ngap_id,ran_ue_old->amf_ue_ngap_id);
                            //ran_ue_remove_sps(ran_ue_old);                         
                        }
                    }
                else
                    ogs_info("SPS amf_ue->ran_ue is NULL");                
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
				    ogs_info("CM_CONNECTED：ran_ue AMF UE  NGAP ID =======   %lu amf_ue AMF UE  NGAP ID= %lu ",ran_ue->amf_ue_ngap_id,amf_ue->ran_ue->amf_ue_ngap_id);
				    /*printf("CM_CONNECTED [%s]    RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld]",
                            amf_ue->suci, amf_ue->ran_ue->ran_ue_ngap_id,
                            (long long)amf_ue->ran_ue->amf_ue_ngap_id);*/

                    /* De-associate NG with NAS/EMM */
                    ran_ue_deassociate(amf_ue->ran_ue);

                    r =  ngap_send_ran_ue_context_release_command(amf_ue->ran_ue,
                            NGAP_Cause_PR_nas, NGAP_CauseNas_normal_release,
                            NGAP_UE_CTX_REL_NG_CONTEXT_REMOVE, 0);
                    ogs_expect(r == OGS_OK);
                    ogs_assert(r != OGS_ERROR);
                }
                ogs_info("ran_ue->amf_ue_ngap_id:%lu,ran_ue->ran_ue_ngap_id:%d",ran_ue->amf_ue_ngap_id,ran_ue->ran_ue_ngap_id);
                 if(amf_ue->ran_ue)amf_ue_ran_ue_sps_icps_sync(amf_ue,ran_ue);                
                amf_ue_associate_ran_ue(amf_ue, ran_ue);
                udp_ini_send_supi_notify(amf_ue);
				ogs_info("SPS amf_ue_associate_ran_ue RAN UE  NGAP ID =======   %d amf_ue AMF UE  NGAP ID= %lu ",amf_ue->ran_ue->ran_ue_ngap_id,amf_ue->ran_ue->amf_ue_ngap_id);
                CLEAR_AMF_UE_TIMER(amf_ue->mobile_reachable);
                CLEAR_AMF_UE_TIMER(amf_ue->implicit_deregistration);
            }
        }
    }


    if (!UserLocationInformation) {
        ogs_error("No UserLocationInformation");
      
        return;
    }

    if (UserLocationInformation->present !=
            NGAP_UserLocationInformation_PR_userLocationInformationNR) {
        ogs_error("Not implemented UserLocationInformation[%d]",
                UserLocationInformation->present);
        
        return;
    }

    if (!NAS_PDU) {
        ogs_error("No NAS_PDU");
       
        return;
    }
   

    UserLocationInformationNR =
        UserLocationInformation->choice.userLocationInformationNR;
    ogs_assert(UserLocationInformationNR);
    ogs_ngap_ASN_to_nr_cgi(
            &UserLocationInformationNR->nR_CGI, &ran_ue->saved.nr_cgi);
    ogs_ngap_ASN_to_5gs_tai(
            &UserLocationInformationNR->tAI, &ran_ue->saved.nr_tai);

    ogs_info("    RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld] "
            "TAC[%d] CellID[0x%llx]",
        ran_ue->ran_ue_ngap_id, (long long)ran_ue->amf_ue_ngap_id,
        ran_ue->saved.nr_tai.tac.v, (long long)ran_ue->saved.nr_cgi.cell_id);

    if (UEContextRequest) {
        if (*UEContextRequest == NGAP_UEContextRequest_requested) {
            ran_ue->ue_context_requested = true;
            ogs_info("ngap_handle_initial_ue_message_sps ran_ue->ue_context_requested = true");
        }
    }

    ngap_send_to_nas(ran_ue, NGAP_ProcedureCode_id_InitialUEMessage, NAS_PDU);
}

void ngap_handle_ue_radio_capability_info_indication_sps(
    ran_ue_t * ran_ue,ogs_ngap_message_t *message)
{
    char buf[OGS_ADDRSTRLEN];
    int i;
  
    uint64_t amf_ue_ngap_id;

    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_UERadioCapabilityInfoIndication_t
        *UERadioCapabilityInfoIndication = NULL;

    NGAP_UERadioCapabilityInfoIndicationIEs_t *ie = NULL;
    NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
    NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
    NGAP_UERadioCapability_t *UERadioCapability = NULL;

    ogs_assert(message);
    initiatingMessage = message->choice.initiatingMessage;
    ogs_assert(initiatingMessage);
    UERadioCapabilityInfoIndication =
        &initiatingMessage->value.choice.UERadioCapabilityInfoIndication;
    ogs_assert(UERadioCapabilityInfoIndication);

    ogs_debug("UERadioCapabilityInfoIndication");

    for (i = 0;
            i < UERadioCapabilityInfoIndication->protocolIEs.list.count; i++) {
        ie = UERadioCapabilityInfoIndication->protocolIEs.list.array[i];
        switch (ie->id) {
        case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
            RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
            AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_UERadioCapability:
            UERadioCapability = &ie->value.choice.UERadioCapability;
            break;
        default:
            break;
        }
    }


    if (!AMF_UE_NGAP_ID) {
        ogs_error("No AMF_UE_NGAP_ID");

        return;
    }

    if (asn_INTEGER2ulong(AMF_UE_NGAP_ID,
                (unsigned long *)&amf_ue_ngap_id) != 0) {
        ogs_error("Invalid AMF_UE_NGAP_ID");

        return;
    }

    ogs_debug("    RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld]",
            ran_ue->ran_ue_ngap_id, (long long)ran_ue->amf_ue_ngap_id);

    if (ran_ue->amf_ue)
        OGS_ASN_STORE_DATA(&ran_ue->amf_ue->ueRadioCapability,
                UERadioCapability);
}

void ngap_handle_initial_context_setup_response_sps(
        ran_ue_t *ran_ue,ogs_ngap_message_t *message)
{
    char buf[OGS_ADDRSTRLEN];
    int i,r;

    amf_ue_t *amf_ue = NULL;
    //ran_ue_t *ran_ue = NULL;
    amf_sess_t *sess = NULL;
    uint64_t amf_ue_ngap_id = 0;
    amf_nsmf_pdusession_sm_context_param_t param;

    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    NGAP_InitialContextSetupResponse_t *InitialContextSetupResponse = NULL;

    NGAP_InitialContextSetupResponseIEs_t *ie = NULL;
    NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
    NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
    NGAP_PDUSessionResourceSetupListCxtRes_t *PDUSessionList = NULL;
    NGAP_PDUSessionResourceSetupItemCxtRes_t *PDUSessionItem = NULL;
    OCTET_STRING_t *transfer = NULL;   

    ogs_assert(message);
    successfulOutcome = message->choice.successfulOutcome;
    ogs_assert(successfulOutcome);
    InitialContextSetupResponse =
        &successfulOutcome->value.choice.InitialContextSetupResponse;
    ogs_assert(InitialContextSetupResponse);

    ogs_debug("InitialContextSetupResponse");

    for (i = 0; i < InitialContextSetupResponse->protocolIEs.list.count; i++) {
        ie = InitialContextSetupResponse->protocolIEs.list.array[i];
        switch (ie->id) {
        case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
            RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
            AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_PDUSessionResourceSetupListCxtRes:
            PDUSessionList =
                &ie->value.choice.PDUSessionResourceSetupListCxtRes;
            break;
        default:
            break;
        }
    }



    amf_ue = ran_ue->amf_ue;
    if (!amf_ue) {
        ogs_error("Cannot find AMF-UE Context [%lld]",
                (long long)amf_ue_ngap_id);
        r = ngap_send_error_indication_sps(
                ran_ue,
                NGAP_Cause_PR_radioNetwork,
                NGAP_CauseRadioNetwork_unknown_local_UE_NGAP_ID);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    for (i = 0; PDUSessionList && i < PDUSessionList->list.count; i++) {
        PDUSessionItem = (NGAP_PDUSessionResourceSetupItemCxtRes_t *)
            PDUSessionList->list.array[i];

        if (!PDUSessionItem) {
            ogs_error("No PDUSessionResourceSetupItemCxtRes");
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        transfer = &PDUSessionItem->pDUSessionResourceSetupResponseTransfer;
        if (!transfer) {
            ogs_error("No PDUSessionResourceSetupResponseTransfer");
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        if (PDUSessionItem->pDUSessionID ==
                OGS_NAS_PDU_SESSION_IDENTITY_UNASSIGNED) {
            ogs_error("PDU Session Identity is unassigned");
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        sess = amf_sess_find_by_psi(amf_ue, PDUSessionItem->pDUSessionID);
        if (!sess) {
            ogs_error("Cannot find PDU Session ID [%d]",
                    (int)PDUSessionItem->pDUSessionID);
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_radioNetwork,
                    NGAP_CauseRadioNetwork_unknown_PDU_session_ID);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        if (!SESSION_CONTEXT_IN_SMF(sess)) {
            ogs_error("Session Context is not in SMF [%d]",
                    (int)PDUSessionItem->pDUSessionID);
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_radioNetwork,
                    NGAP_CauseRadioNetwork_unknown_PDU_session_ID);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }
        ogs_debug("    SUPI[%s] PSI[%d] OLD ACTIVATED[0x%x]",
                amf_ue->supi, sess->psi, ran_ue->psimask.activated);
        ran_ue->psimask.activated |= ((1 << sess->psi));
        ogs_debug("    NEW ACTIVATED[0x%x]", ran_ue->psimask.activated);

        memset(&param, 0, sizeof(param));
        param.n2smbuf = ogs_pkbuf_alloc(NULL, OGS_MAX_SDU_LEN);
        ogs_assert(param.n2smbuf);
        param.n2SmInfoType = OpenAPI_n2_sm_info_type_PDU_RES_SETUP_RSP;
        ogs_pkbuf_put_data(param.n2smbuf, transfer->buf, transfer->size);

        r= amf_sess_sbi_discover_and_send(
                OGS_SBI_SERVICE_TYPE_NSMF_PDUSESSION, NULL,
                amf_nsmf_pdusession_build_update_sm_context,
                sess, AMF_UPDATE_SM_CONTEXT_ACTIVATED, &param);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);

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
#if 0
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

 #endif
                r = nas_send_pdu_session_modification_command(sess,
                            sess->gsm_message.n1buf, sess->gsm_message.n2buf);
                ogs_expect(r == OGS_OK);
                ogs_assert(r != OGS_ERROR);
                /* n1buf is de-allocated
                 * in gmm_build_dl_nas_transport() */
                sess->gsm_message.n1buf = NULL;
                /* n2buf is de-allocated
                 * in ngap_build_pdu_session_resource_modify_request() */
                sess->gsm_message.n2buf = NULL;

                AMF_SESS_CLEAR_5GSM_MESSAGE(sess);

                break;
            case OGS_NAS_5GS_PDU_SESSION_RELEASE_COMMAND:
                r = nas_send_pdu_session_release_command(sess,
                            sess->gsm_message.n1buf, sess->gsm_message.n2buf);
                ogs_expect(r == OGS_OK);
                ogs_assert(r != OGS_ERROR);

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
         *    for mobility registration update; and
         * c) after a successful service request procedure invoked
         *    as a response to a paging request from the network and
         *    before the release of the N1 NAS signalling connection
         *    as specified in subclause 5.4.4.1.
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
        r=nas_5gs_send_configuration_update_command(amf_ue, &param);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);

        AMF_UE_CLEAR_PAGING_INFO(amf_ue);
    }
}

void ngap_handle_initial_context_setup_failure_sps(
        ran_ue_t *ran_ue,ogs_ngap_message_t *message)
{
    char buf[OGS_ADDRSTRLEN];
    int i,r, old_xact_count = 0, new_xact_count = 0;

   
    amf_ue_t *amf_ue = NULL;
    uint64_t amf_ue_ngap_id;

    NGAP_UnsuccessfulOutcome_t *unsuccessfulOutcome = NULL;
    NGAP_InitialContextSetupFailure_t *InitialContextSetupFailure = NULL;

    NGAP_InitialContextSetupFailureIEs_t *ie = NULL;
    NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
    NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
    NGAP_Cause_t *Cause = NULL;


    ogs_assert(message);
    unsuccessfulOutcome = message->choice.unsuccessfulOutcome;
    ogs_assert(unsuccessfulOutcome);
    InitialContextSetupFailure =
        &unsuccessfulOutcome->value.choice.InitialContextSetupFailure;
    ogs_assert(InitialContextSetupFailure);

    ogs_debug("InitialContextSetupFailure");

    for (i = 0; i < InitialContextSetupFailure->protocolIEs.list.count; i++) {
        ie = InitialContextSetupFailure->protocolIEs.list.array[i];
        switch (ie->id) {
        case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
            RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
            AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_Cause:
            Cause = &ie->value.choice.Cause;
            break;
        default:
            break;
        }
    }


    /*
     * 19.2.2.3 in Spec 36.300
     *
     * In case of failure, RAN and AMF behaviours are not mandated.
     *
     * Both implicit release (local release at each node) and
     * explicit release (AMF-initiated UE Context Release procedure)
     * may in principle be adopted. The RAN should ensure
     * that no hanging resources remain at the RAN.
     */
    amf_ue = ran_ue->amf_ue;
    if (amf_ue) {
        /*
         * if T3550 is running, Registration complete will be sent.
         * So, we need to clear all the timer at this point.
         */
        CLEAR_AMF_UE_ALL_TIMERS(amf_ue);

        old_xact_count = amf_sess_xact_count(amf_ue);

        amf_ue->deactivation.group = NGAP_Cause_PR_nas;
        amf_ue->deactivation.cause = NGAP_CauseNas_normal_release;

        amf_sbi_send_deactivate_all_sessions(
                amf_ue, AMF_UPDATE_SM_CONTEXT_DEACTIVATED,
                Cause->present, (int)Cause->choice.radioNetwork);

        new_xact_count = amf_sess_xact_count(amf_ue);
    }

    if (old_xact_count == new_xact_count) {
        r = ngap_send_ran_ue_context_release_command(ran_ue,
                NGAP_Cause_PR_nas, NGAP_CauseNas_normal_release,
                NGAP_UE_CTX_REL_NG_CONTEXT_REMOVE, 0);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
    }
}

void ngap_handle_pdu_session_resource_setup_response_sps(ran_ue_t * ran_ue,ogs_ngap_message_t *message)
{
	char buf[OGS_ADDRSTRLEN];
	int i,r;
	
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

			r = ngap_send_error_indication_sps(
					ran_ue,
					NGAP_Cause_PR_radioNetwork,
					NGAP_CauseRadioNetwork_unknown_local_UE_NGAP_ID);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);

			return;
		}
	
		if (PDUSessionList) {
			for (i = 0; i < PDUSessionList->list.count; i++) {
				amf_sess_t *sess = NULL;
				PDUSessionItem = (NGAP_PDUSessionResourceSetupItemSURes_t *)
					PDUSessionList->list.array[i];
	
				if (!PDUSessionItem) {
					ogs_error("No PDUSessionResourceSetupItemSURes");

					r = ngap_send_error_indication2_sps(
							amf_ue,
							NGAP_Cause_PR_protocol,
							NGAP_CauseProtocol_semantic_error);
                    ogs_expect(r == OGS_OK);
                    ogs_assert(r != OGS_ERROR);

					return;
				}
	
				transfer = &PDUSessionItem->pDUSessionResourceSetupResponseTransfer;
				if (!transfer) {
					ogs_error("No PDUSessionResourceSetupResponseTransfer");


					r = ngap_send_error_indication2_sps(
							amf_ue,
							NGAP_Cause_PR_protocol,
							NGAP_CauseProtocol_semantic_error);
                    ogs_expect(r == OGS_OK);
                    ogs_assert(r != OGS_ERROR);


					return;
				}
	
				if (PDUSessionItem->pDUSessionID ==
						OGS_NAS_PDU_SESSION_IDENTITY_UNASSIGNED) {
					ogs_error("PDU Session Identity is unassigned");

					r = ngap_send_error_indication2_sps(
							amf_ue,
							NGAP_Cause_PR_protocol,
							NGAP_CauseProtocol_semantic_error);
                    ogs_expect(r == OGS_OK);
                    ogs_assert(r != OGS_ERROR);

					return;
				}
	
				sess = amf_sess_find_by_psi(amf_ue, PDUSessionItem->pDUSessionID);
				if (!sess) {
					ogs_error("Cannot find PDU Session ID [%d]",
							(int)PDUSessionItem->pDUSessionID);

					r = ngap_send_error_indication2_sps(amf_ue,
							NGAP_Cause_PR_radioNetwork,
							NGAP_CauseRadioNetwork_unknown_PDU_session_ID);
                    ogs_expect(r == OGS_OK);
                    ogs_assert(r != OGS_ERROR);

					return;
				}
	
				if (!SESSION_CONTEXT_IN_SMF(sess)) {
					ogs_error("Session Context is not in SMF [%d]",
							(int)PDUSessionItem->pDUSessionID);


					r = ngap_send_error_indication2_sps(amf_ue,
							NGAP_Cause_PR_radioNetwork,
							NGAP_CauseRadioNetwork_unknown_PDU_session_ID);
                    ogs_expect(r == OGS_OK);
                    ogs_assert(r != OGS_ERROR);

					return;
				}
	            ogs_debug("    SUPI[%s] PSI[%d] OLD ACTIVATED[0x%x]",
                amf_ue->supi, sess->psi, ran_ue->psimask.activated);
                ran_ue->psimask.activated |= ((1 << sess->psi));
                ogs_debug("    NEW ACTIVATED[0x%x]", ran_ue->psimask.activated);
				memset(&param, 0, sizeof(param));
				param.n2smbuf = ogs_pkbuf_alloc(NULL, OGS_MAX_SDU_LEN);
				ogs_assert(param.n2smbuf);
				param.n2SmInfoType = OpenAPI_n2_sm_info_type_PDU_RES_SETUP_RSP;
				ogs_pkbuf_put_data(param.n2smbuf, transfer->buf, transfer->size);
	
				r = amf_sess_sbi_discover_and_send(
						OGS_SBI_SERVICE_TYPE_NSMF_PDUSESSION, NULL,
						amf_nsmf_pdusession_build_update_sm_context,
						sess, AMF_UPDATE_SM_CONTEXT_ACTIVATED, &param);
                ogs_expect(r == OGS_OK);
                ogs_assert(r != OGS_ERROR);
	
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
					r = ngap_send_error_indication2_sps(
							amf_ue,
							NGAP_Cause_PR_protocol,
							NGAP_CauseProtocol_semantic_error);
                    ogs_expect(r == OGS_OK);
                    ogs_assert(r != OGS_ERROR);
					return;
				}
	
				transfer =
					&PDUSessionFailedItem->
						pDUSessionResourceSetupUnsuccessfulTransfer;
				if (!transfer) {
					ogs_error("No PDUSessionResourceSetupUnsuccessfulTransfer");
					r = ngap_send_error_indication2_sps(
							amf_ue,
							NGAP_Cause_PR_protocol,
							NGAP_CauseProtocol_semantic_error);
                    ogs_expect(r == OGS_OK);
                    ogs_assert(r != OGS_ERROR);
					return;
				}
	
				if (PDUSessionFailedItem->pDUSessionID ==
						OGS_NAS_PDU_SESSION_IDENTITY_UNASSIGNED) {
					ogs_error("PDU Session Identity is unassigned");
					r = ngap_send_error_indication2_sps(
							amf_ue,
							NGAP_Cause_PR_protocol,
							NGAP_CauseProtocol_semantic_error);
                    ogs_expect(r == OGS_OK);
                    ogs_assert(r != OGS_ERROR);
					return;
				}
	
				sess = amf_sess_find_by_psi(
						amf_ue, PDUSessionFailedItem->pDUSessionID);
				if (!sess) {
					ogs_error("Cannot find PDU Session ID [%d]",
							(int)PDUSessionFailedItem->pDUSessionID);
					r = ngap_send_error_indication2_sps(
							amf_ue,
							NGAP_Cause_PR_radioNetwork,
							NGAP_CauseRadioNetwork_unknown_PDU_session_ID);
                    ogs_expect(r == OGS_OK);
                    ogs_assert(r != OGS_ERROR);
					return;
				}
	
				if (!SESSION_CONTEXT_IN_SMF(sess)) {
					ogs_error("Session Context is not in SMF [%d]",
							(int)PDUSessionFailedItem->pDUSessionID);
					r = ngap_send_error_indication2_sps(amf_ue,
							NGAP_Cause_PR_radioNetwork,
							NGAP_CauseRadioNetwork_unknown_PDU_session_ID);
                    ogs_expect(r == OGS_OK);
                    ogs_assert(r != OGS_ERROR);
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
	
				r = amf_sess_sbi_discover_and_send(
						OGS_SBI_SERVICE_TYPE_NSMF_PDUSESSION, NULL,
						amf_nsmf_pdusession_build_update_sm_context,
						sess, AMF_UPDATE_SM_CONTEXT_SETUP_FAIL, &param);
                ogs_expect(r == OGS_OK);
                ogs_assert(r != OGS_ERROR);
	
				ogs_pkbuf_free(param.n2smbuf);
			}
		} else {
			ogs_error("No PDUSessionResourceList");
			r = ngap_send_error_indication2_sps(amf_ue,
					NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
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
void ngap_handle_ue_context_release_request_sps(
        ran_ue_t *ran_ue,ogs_ngap_message_t *message)
{
    int i,r;
    char buf[OGS_ADDRSTRLEN];
    uint64_t amf_ue_ngap_id = 0;

    //ran_ue_t *ran_ue = NULL;
    amf_ue_t *amf_ue = NULL;
    amf_sess_t *sess = NULL;

    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_UEContextReleaseRequest_t *UEContextReleaseRequest = NULL;

    NGAP_UEContextReleaseRequest_IEs_t *ie = NULL;
    NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
    NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
    NGAP_PDUSessionResourceListCxtRelReq_t *PDUSessionList = NULL;
    NGAP_PDUSessionResourceItemCxtRelReq_t *PDUSessionItem = NULL;
    NGAP_Cause_t *Cause = NULL;
	NGAP_Cause_sps_t *Trancause=NULL;
	

    ogs_assert(message);
    initiatingMessage = message->choice.initiatingMessage;
    ogs_assert(initiatingMessage);
    UEContextReleaseRequest =
        &initiatingMessage->value.choice.UEContextReleaseRequest;
    ogs_assert(UEContextReleaseRequest);

    ogs_debug("UEContextReleaseRequest");

    for (i = 0; i < UEContextReleaseRequest->protocolIEs.list.count; i++) {
        ie = UEContextReleaseRequest->protocolIEs.list.array[i];
        switch (ie->id) {
        case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
            RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
            AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_PDUSessionResourceListCxtRelReq:
            PDUSessionList = &ie->value.choice.PDUSessionResourceListCxtRelReq;
            break;
        case NGAP_ProtocolIE_ID_id_Cause:
            Cause = &ie->value.choice.Cause;
            break;
        default:
            break;
        }
    }

    amf_ue = ran_ue->amf_ue;
    if (!amf_ue) {
        ogs_error("Cannot find AMF-UE Context [%lld]",
                (long long)amf_ue_ngap_id);
        r = ngap_send_ran_ue_context_release_command(ran_ue,
                Cause->present, (int)Cause->choice.radioNetwork,
                NGAP_UE_CTX_REL_NG_CONTEXT_REMOVE, 0);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
    } else {
        int xact_count = amf_sess_xact_count(amf_ue);

        amf_ue->deactivation.group = Cause->present;
        amf_ue->deactivation.cause = (int)Cause->choice.radioNetwork;

        if (!PDUSessionList) {
            amf_sbi_send_deactivate_all_sessions(
                    amf_ue, AMF_UPDATE_SM_CONTEXT_DEACTIVATED,
                    Cause->present, (int)Cause->choice.radioNetwork);
        } else {
            for (i = 0; i < PDUSessionList->list.count; i++) {
                PDUSessionItem = (NGAP_PDUSessionResourceItemCxtRelReq_t *)
                    PDUSessionList->list.array[i];

                if (!PDUSessionItem) {
                    ogs_error("No PDUSessionResourceSetupItemSURes");
                    r = ngap_send_error_indication2_sps(
                            amf_ue, NGAP_Cause_PR_protocol,
                            NGAP_CauseProtocol_semantic_error);
                    ogs_expect(r == OGS_OK);
                    ogs_assert(r != OGS_ERROR);
                    return;
                }

                if (PDUSessionItem->pDUSessionID ==
                        OGS_NAS_PDU_SESSION_IDENTITY_UNASSIGNED) {
                    ogs_error("PDU Session Identity is unassigned");
                    r = ngap_send_error_indication2_sps(
                            amf_ue, NGAP_Cause_PR_protocol,
                            NGAP_CauseProtocol_semantic_error);
                    ogs_expect(r == OGS_OK);
                    ogs_assert(r != OGS_ERROR);
                    return;
                }

                sess = amf_sess_find_by_psi(amf_ue,
                        PDUSessionItem->pDUSessionID);
                if (SESSION_CONTEXT_IN_SMF(sess)) {
                    amf_sbi_send_deactivate_session(
                            sess, AMF_UPDATE_SM_CONTEXT_DEACTIVATED,
                            Cause->present, (int)Cause->choice.radioNetwork);
                }
				
            }
        }

        if (amf_sess_xact_count(amf_ue) == xact_count){
            r = ngap_send_amf_ue_context_release_command(amf_ue,
                    Cause->present, (int)Cause->choice.radioNetwork,
                    NGAP_UE_CTX_REL_NG_REMOVE_AND_UNLINK, 0);            
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
    }
    }

	
}

void ngap_handle_pdu_session_resource_release_response_sps(
        ran_ue_t *ran_ue,ogs_ngap_message_t *message)
{
    char buf[OGS_ADDRSTRLEN];
    int i,r;

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
        r = ngap_send_error_indication_sps(ran_ue, 
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    if (asn_INTEGER2ulong(AMF_UE_NGAP_ID,
                (unsigned long *)&amf_ue_ngap_id) != 0) {
        ogs_error("Invalid AMF_UE_NGAP_ID");
        r = ngap_send_error_indication_sps(ran_ue, 
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    ran_ue = ran_ue_find_by_amf_ue_ngap_id_sps(&amf_ue_ngap_id);
    if (!ran_ue) {
        ogs_error("No RAN UE Context : AMF_UE_NGAP_ID[%lld]",
                (long long)amf_ue_ngap_id);
        r = ngap_send_error_indication_sps(
                ran_ue,
                NGAP_Cause_PR_radioNetwork,
                NGAP_CauseRadioNetwork_unknown_local_UE_NGAP_ID);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    ogs_debug("    RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld]",
            ran_ue->ran_ue_ngap_id, (long long)ran_ue->amf_ue_ngap_id);

    amf_ue = ran_ue->amf_ue;
    if (!amf_ue) {
        ogs_error("Cannot find AMF-UE Context [%lld]",
                (long long)amf_ue_ngap_id);
        r = ngap_send_error_indication_sps(
                ran_ue,
                NGAP_Cause_PR_radioNetwork,
                NGAP_CauseRadioNetwork_unknown_local_UE_NGAP_ID);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    if (!PDUSessionList) {
        ogs_error("No PDUSessionResourceReleasedListRelRes");
        r = ngap_send_error_indication2_sps(amf_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    for (i = 0; i < PDUSessionList->list.count; i++) {
        amf_sess_t *sess = NULL;
        PDUSessionItem = (NGAP_PDUSessionResourceReleasedItemRelRes_t *)
            PDUSessionList->list.array[i];

        if (!PDUSessionItem) {
            ogs_error("No PDUSessionResourceReleasedItemRelRes");
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        transfer = &PDUSessionItem->pDUSessionResourceReleaseResponseTransfer;
        if (!transfer) {
            ogs_error("No PDUSessionResourceReleaseResponseTransfer");
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        if (PDUSessionItem->pDUSessionID ==
                OGS_NAS_PDU_SESSION_IDENTITY_UNASSIGNED) {
            ogs_error("PDU Session Identity is unassigned");
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        sess = amf_sess_find_by_psi(amf_ue, PDUSessionItem->pDUSessionID);
        if (!sess) {
            ogs_error("Cannot find PDU Session ID [%d]",
                    (int)PDUSessionItem->pDUSessionID);
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_radioNetwork,
                    NGAP_CauseRadioNetwork_unknown_PDU_session_ID);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        if (!SESSION_CONTEXT_IN_SMF(sess)) {
            ogs_error("Session Context is not in SMF [%d]",
                    (int)PDUSessionItem->pDUSessionID);
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_radioNetwork,
                    NGAP_CauseRadioNetwork_unknown_PDU_session_ID);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        memset(&param, 0, sizeof(param));
        param.n2smbuf = ogs_pkbuf_alloc(NULL, OGS_MAX_SDU_LEN);
        ogs_assert(param.n2smbuf);
        param.n2SmInfoType = OpenAPI_n2_sm_info_type_PDU_RES_REL_RSP;
        ogs_pkbuf_put_data(param.n2smbuf, transfer->buf, transfer->size);

        r = amf_sess_sbi_discover_and_send(
                OGS_SBI_SERVICE_TYPE_NSMF_PDUSESSION, NULL,
                amf_nsmf_pdusession_build_update_sm_context,
                sess, AMF_UPDATE_SM_CONTEXT_N2_RELEASED, &param);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);

        ogs_pkbuf_free(param.n2smbuf);
    }
}
void ngap_handle_handover_required_sps(
       uint64_t target_amf_ue_ngap_id, ran_ue_t *ran_ue, ogs_ngap_message_t *message)
{
    char buf[OGS_ADDRSTRLEN];
    int i,r;

    amf_ue_t *amf_ue = NULL;
    ran_ue_t *source_ue = NULL, *target_ue = NULL;
    uint64_t amf_ue_ngap_id;

    amf_gnb_t *target_gnb = NULL;
    uint32_t target_gnb_id;

    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_HandoverRequired_t *HandoverRequired = NULL;

    NGAP_HandoverRequiredIEs_t *ie = NULL;
    NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
    NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
    NGAP_HandoverType_t *HandoverType = NULL;
    NGAP_Cause_t *Cause = NULL;
    NGAP_TargetID_t *TargetID = NULL;
    NGAP_TargetRANNodeID_t *targetRANNodeID = NULL;
    NGAP_GlobalRANNodeID_t *globalRANNodeID = NULL;
    NGAP_GlobalGNB_ID_t *globalGNB_ID = NULL;
    NGAP_PDUSessionResourceListHORqd_t *PDUSessionList = NULL;
    NGAP_PDUSessionResourceItemHORqd_t *PDUSessionItem = NULL;
    OCTET_STRING_t *transfer = NULL;

    NGAP_SourceToTarget_TransparentContainer_t
        *SourceToTarget_TransparentContainer = NULL;

    amf_nsmf_pdusession_sm_context_param_t param;



    ogs_assert(message);
    initiatingMessage = message->choice.initiatingMessage;
    ogs_assert(initiatingMessage);
    HandoverRequired = &initiatingMessage->value.choice.HandoverRequired;
    ogs_assert(HandoverRequired);

    ogs_info("HandoverRequired");

    for (i = 0; i < HandoverRequired->protocolIEs.list.count; i++) {
        ie = HandoverRequired->protocolIEs.list.array[i];
        switch (ie->id) {
        case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
            RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
            AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_HandoverType:
            HandoverType = &ie->value.choice.HandoverType;
            break;
        case NGAP_ProtocolIE_ID_id_Cause:
            Cause = &ie->value.choice.Cause;
            break;
        case NGAP_ProtocolIE_ID_id_TargetID:
            TargetID = &ie->value.choice.TargetID;
            break;
        case NGAP_ProtocolIE_ID_id_PDUSessionResourceListHORqd:
            PDUSessionList = &ie->value.choice.PDUSessionResourceListHORqd;
            break;
        case NGAP_ProtocolIE_ID_id_SourceToTarget_TransparentContainer:
            SourceToTarget_TransparentContainer =
                &ie->value.choice.SourceToTarget_TransparentContainer;
            break;
        default:
            break;
        }
    }

    if (!AMF_UE_NGAP_ID) {
    ogs_error("No AMF_UE_NGAP_ID");

    return;
    }

    if (asn_INTEGER2ulong(AMF_UE_NGAP_ID,
                (unsigned long *)&amf_ue_ngap_id) != 0) {
        ogs_error("Invalid AMF_UE_NGAP_ID");
   
        return;
    }

    //source_ue = ran_ue_find_by_amf_ue_ngap_id(amf_ue_ngap_id);
    source_ue=ran_ue;
    if (!source_ue) {
        ogs_error("No RAN UE Context : AMF_UE_NGAP_ID[%lld]",
                (long long)amf_ue_ngap_id);
        
        return;
    }
    ogs_debug("    Source : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld] ",
        source_ue->ran_ue_ngap_id, (long long)source_ue->amf_ue_ngap_id);

    amf_ue = source_ue->amf_ue;
    if (!amf_ue) {
        ogs_error("Cannot find AMF-UE Context [%lld]",
                (long long)amf_ue_ngap_id);
        r = ngap_send_error_indication_sps(
                source_ue,
                NGAP_Cause_PR_radioNetwork,
                NGAP_CauseRadioNetwork_unknown_local_UE_NGAP_ID);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

   if (!HandoverType) {
        ogs_error("No HandoverType");
        r = ngap_send_error_indication2_sps(amf_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    if (!Cause) {
        ogs_error("No Cause");
        r = ngap_send_error_indication2_sps(amf_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    if (!TargetID) {
        ogs_error("No TargetID");
        r = ngap_send_error_indication2_sps(amf_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    if (TargetID->present != NGAP_TargetID_PR_targetRANNodeID) {
        ogs_error("Not implemented TargetID[%d]", TargetID->present);
        r = ngap_send_error_indication2_sps(amf_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }
    #if 0
    targetRANNodeID = TargetID->choice.targetRANNodeID;
    if (!targetRANNodeID) {
        ogs_error("No targetRANNodeID");
        ogs_assert(OGS_OK ==
            ngap_send_error_indication2_sps(amf_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error));
        return;
    }

    globalRANNodeID = &targetRANNodeID->globalRANNodeID;
    if (globalRANNodeID->present != NGAP_GlobalRANNodeID_PR_globalGNB_ID) {
        ogs_error("Not implemented globalRANNodeID[%d]",
                globalRANNodeID->present);
        ogs_assert(OGS_OK ==
            ngap_send_error_indication2_sps(amf_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error));
        return;
    }

    globalGNB_ID = globalRANNodeID->choice.globalGNB_ID;
    if (!globalGNB_ID) {
        ogs_error("No globalGNB_ID");
        ogs_assert(OGS_OK ==
            ngap_send_error_indication2_sps(amf_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error));
        return;
    }

    ogs_ngap_GNB_ID_to_uint32(&globalGNB_ID->gNB_ID, &target_gnb_id);
    target_gnb = amf_gnb_find_by_gnb_id(target_gnb_id);
    if (!target_gnb) {
        ogs_error("Handover required : cannot find target gNB-id[0x%x]",
                target_gnb_id);
        ogs_assert(OGS_OK ==
            ngap_send_error_indication2_sps(amf_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error));
        return;
    }
#endif
    if (!PDUSessionList) {
        ogs_error("No PDUSessionList");
#if 0
        r = ngap_send_error_indication2_sps(amf_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
#endif
        return;
    }

    if (!SourceToTarget_TransparentContainer) {
        ogs_error("No SourceToTarget_TransparentContainer");
#if 0
        r = ngap_send_error_indication2_sps(amf_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
#endif
        return;
    }
#if 0
    if (!SECURITY_CONTEXT_IS_VALID(amf_ue)) {
        ogs_error("No Security Context");
        ogs_assert(OGS_OK ==
            ngap_send_error_indication2_sps(amf_ue,
                NGAP_Cause_PR_nas, NGAP_CauseNas_authentication_failure));
        return;
    }
#endif
#if 0
    /* Target UE */
    target_ue = ran_ue_add(target_gnb, INVALID_UE_NGAP_ID);
    if (target_ue == NULL) {
        ogs_assert(OGS_OK ==
            ngap_send_error_indication2_sps(amf_ue, NGAP_Cause_PR_misc,
                NGAP_CauseMisc_control_processing_overload));
        return;
    }
#endif
    /* Target UE add O3 0323 */
       target_ue = ran_ue_add_sps(INVALID_UE_NGAP_ID,target_amf_ue_ngap_id);
       if (target_ue == NULL) {
           r = ngap_send_error_indication2_sps(amf_ue, NGAP_Cause_PR_misc,
                   NGAP_CauseMisc_control_processing_overload);
           ogs_expect(r == OGS_OK);
           ogs_assert(r != OGS_ERROR);
           return;
       }

    /* Source UE - Target UE associated */
    source_ue_associate_target_ue(source_ue, target_ue);

    /* Context Transfer */
    target_ue->ue_context_requested = source_ue->ue_context_requested;
    target_ue->initial_context_setup_request_sent =
            source_ue->initial_context_setup_request_sent;
    target_ue->psimask.activated = source_ue->psimask.activated;

    ogs_debug("    Target : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld] ",
        target_ue->ran_ue_ngap_id, (long long)target_ue->amf_ue_ngap_id);

    /* Store HandoverType */
    amf_ue->handover.type = *HandoverType;

    /* Store Cause */
    amf_ue->handover.group = Cause->present;
    amf_ue->handover.cause = (int)Cause->choice.radioNetwork;

    /* Update Security Context (NextHop) */
    amf_ue->nhcc++;
    ogs_kdf_nh_gnb(amf_ue->kamf, amf_ue->nh, amf_ue->nh);

    /* Store Container */
    OGS_ASN_STORE_DATA(&amf_ue->handover.container,
            SourceToTarget_TransparentContainer);

    for (i = 0; i < PDUSessionList->list.count; i++) {
        amf_sess_t *sess = NULL;
        PDUSessionItem = (NGAP_PDUSessionResourceItemHORqd_t *)
            PDUSessionList->list.array[i];

        if (!PDUSessionItem) {
            ogs_error("No PDUSessionResourceItemHORqd");
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        transfer = &PDUSessionItem->handoverRequiredTransfer;
        if (!transfer) {
            ogs_error("No handoverRequiredTransfer");
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        if (PDUSessionItem->pDUSessionID ==
                OGS_NAS_PDU_SESSION_IDENTITY_UNASSIGNED) {
            ogs_error("PDU Session Identity is unassigned");
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        sess = amf_sess_find_by_psi(amf_ue, PDUSessionItem->pDUSessionID);
        if (!sess) {
            ogs_error("Cannot find PDU Session ID [%d]",
                    (int)PDUSessionItem->pDUSessionID);
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_radioNetwork,
                    NGAP_CauseRadioNetwork_unknown_PDU_session_ID);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        if (!SESSION_CONTEXT_IN_SMF(sess)) {
            ogs_error("Session Context is not in SMF [%d]",
                    (int)PDUSessionItem->pDUSessionID);
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_radioNetwork,
                    NGAP_CauseRadioNetwork_unknown_PDU_session_ID);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        memset(&param, 0, sizeof(param));
        param.n2smbuf = ogs_pkbuf_alloc(NULL, OGS_MAX_SDU_LEN);
        ogs_assert(param.n2smbuf);
        param.n2SmInfoType = OpenAPI_n2_sm_info_type_HANDOVER_REQUIRED;
        ogs_pkbuf_put_data(param.n2smbuf, transfer->buf, transfer->size);

        param.hoState = OpenAPI_ho_state_PREPARING;
        param.TargetID = TargetID;

        r = amf_sess_sbi_discover_and_send(
                OGS_SBI_SERVICE_TYPE_NSMF_PDUSESSION, NULL,
                amf_nsmf_pdusession_build_update_sm_context,
                sess, AMF_UPDATE_SM_CONTEXT_HANDOVER_REQUIRED, &param);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);

        ogs_pkbuf_free(param.n2smbuf);
    }
}
void ngap_handle_handover_request_ack_sps(
        ran_ue_t *ran_ue, ogs_ngap_message_t *message)
{
    char buf[OGS_ADDRSTRLEN];
    int i,r;

    amf_ue_t *amf_ue = NULL;
    ran_ue_t *source_ue = NULL, *target_ue = NULL;
    uint64_t amf_ue_ngap_id;
    amf_nsmf_pdusession_sm_context_param_t param;

    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    NGAP_HandoverRequestAcknowledge_t *HandoverRequestAcknowledge = NULL;

    NGAP_HandoverRequestAcknowledgeIEs_t *ie = NULL;
    NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
    NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
    NGAP_PDUSessionResourceAdmittedList_t *PDUSessionList = NULL;
    NGAP_PDUSessionResourceAdmittedItem_t *PDUSessionItem = NULL;
    OCTET_STRING_t *transfer = NULL;

    NGAP_TargetToSource_TransparentContainer_t
        *TargetToSource_TransparentContainer = NULL;



    ogs_assert(message);
    successfulOutcome = message->choice.successfulOutcome;
    ogs_assert(successfulOutcome);
    HandoverRequestAcknowledge =
        &successfulOutcome->value.choice.HandoverRequestAcknowledge;
    ogs_assert(HandoverRequestAcknowledge);

    ogs_debug("HandoverRequestAcknowledge");

    for (i = 0; i < HandoverRequestAcknowledge->protocolIEs.list.count; i++) {
        ie = HandoverRequestAcknowledge->protocolIEs.list.array[i];
        switch (ie->id) {
        case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
            RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
            AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_PDUSessionResourceAdmittedList:
            PDUSessionList = &ie->value.choice.PDUSessionResourceAdmittedList;
            break;
        case NGAP_ProtocolIE_ID_id_TargetToSource_TransparentContainer:
            TargetToSource_TransparentContainer =
                &ie->value.choice.TargetToSource_TransparentContainer;
            break;
        default:
            break;
        }
    }

   
    if (!RAN_UE_NGAP_ID) {
        ogs_error("No RAN_UE_NGAP_ID");
        
        return;
    }

    if (!AMF_UE_NGAP_ID) {
        ogs_error("No AMF_UE_NGAP_ID");
        
        return;
    }

    if (asn_INTEGER2ulong(AMF_UE_NGAP_ID,
                (unsigned long *)&amf_ue_ngap_id) != 0) {
        ogs_error("Invalid AMF_UE_NGAP_ID");
       
        return;
    }
    target_ue = ran_ue_find_by_amf_ue_ngap_id_sps(&amf_ue_ngap_id);
    if (!target_ue) {
        ogs_error("No RAN UE Context : AMF_UE_NGAP_ID[%lld]",
                (long long)amf_ue_ngap_id);
    
        return;
    }


    target_ue->ran_ue_ngap_id = *RAN_UE_NGAP_ID;

    source_ue = target_ue->source_ue;
    if (!source_ue) {
        ogs_error("Cannot find Source-UE Context [%lld]",
                (long long)amf_ue_ngap_id);
       
        return;
    }
    amf_ue = target_ue->amf_ue;
    if (!amf_ue) {
        ogs_error("Cannot find AMF-UE Context [%lld]",
                (long long)amf_ue_ngap_id);
        r = ngap_send_error_indication_sps(
                target_ue,
                NGAP_Cause_PR_radioNetwork,
                NGAP_CauseRadioNetwork_unknown_local_UE_NGAP_ID);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    ogs_debug("    Source : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld] ",
        source_ue->ran_ue_ngap_id, (long long)source_ue->amf_ue_ngap_id);
    ogs_debug("    Target : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld] ",
        target_ue->ran_ue_ngap_id, (long long)target_ue->amf_ue_ngap_id);

  
    /* Store Container */
    OGS_ASN_STORE_DATA(&amf_ue->handover.container,
            TargetToSource_TransparentContainer);

    for (i = 0; i < PDUSessionList->list.count; i++) {
        amf_sess_t *sess = NULL;
        PDUSessionItem = (NGAP_PDUSessionResourceAdmittedItem_t *)
            PDUSessionList->list.array[i];

        if (!PDUSessionItem) {
            ogs_error("No PDUSessionResourceAdmittedItem");
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        transfer = &PDUSessionItem->handoverRequestAcknowledgeTransfer;
        if (!transfer) {
            ogs_error("No handoverRequestAcknowledgeTransfer");
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        if (PDUSessionItem->pDUSessionID ==
                OGS_NAS_PDU_SESSION_IDENTITY_UNASSIGNED) {
            ogs_error("PDU Session Identity is unassigned");
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        sess = amf_sess_find_by_psi(amf_ue, PDUSessionItem->pDUSessionID);
        if (!sess) {
            ogs_error("Cannot find PDU Session ID [%d]",
                    (int)PDUSessionItem->pDUSessionID);
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_radioNetwork,
                    NGAP_CauseRadioNetwork_unknown_PDU_session_ID);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        if (!SESSION_CONTEXT_IN_SMF(sess)) {
            ogs_error("Session Context is not in SMF [%d]",
                    (int)PDUSessionItem->pDUSessionID);
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_radioNetwork,
                    NGAP_CauseRadioNetwork_unknown_PDU_session_ID);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        memset(&param, 0, sizeof(param));
        param.n2smbuf = ogs_pkbuf_alloc(NULL, OGS_MAX_SDU_LEN);
        ogs_assert(param.n2smbuf);
        param.n2SmInfoType = OpenAPI_n2_sm_info_type_HANDOVER_REQ_ACK;
        ogs_pkbuf_put_data(param.n2smbuf, transfer->buf, transfer->size);

        param.hoState = OpenAPI_ho_state_PREPARED;

        r = amf_sess_sbi_discover_and_send(
                OGS_SBI_SERVICE_TYPE_NSMF_PDUSESSION, NULL,
                amf_nsmf_pdusession_build_update_sm_context,
                sess, AMF_UPDATE_SM_CONTEXT_HANDOVER_REQ_ACK, &param);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);

        ogs_pkbuf_free(param.n2smbuf);
    }
}

void ngap_handle_handover_failure_sps(
        ran_ue_t *ran_ue, ogs_ngap_message_t *message)
{
    char buf[OGS_ADDRSTRLEN];
    int i,r;

    ran_ue_t *source_ue = NULL, *target_ue = NULL;
    uint64_t amf_ue_ngap_id;

    NGAP_UnsuccessfulOutcome_t *unsuccessfulOutcome = NULL;
    NGAP_HandoverFailure_t *HandoverFailure = NULL;

    NGAP_HandoverFailureIEs_t *ie = NULL;
    NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
    NGAP_Cause_t *Cause = NULL;



    ogs_assert(message);
    unsuccessfulOutcome = message->choice.unsuccessfulOutcome;
    ogs_assert(unsuccessfulOutcome);
    HandoverFailure =
        &unsuccessfulOutcome->value.choice.HandoverFailure;
    ogs_assert(HandoverFailure);

    ogs_debug("HandoverFailure");

    for (i = 0; i < HandoverFailure->protocolIEs.list.count; i++) {
        ie = HandoverFailure->protocolIEs.list.array[i];
        switch (ie->id) {
        case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
            AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_Cause:
            Cause = &ie->value.choice.Cause;
            break;
        default:
            break;
        }
    }


    if (asn_INTEGER2ulong(AMF_UE_NGAP_ID,
                (unsigned long *)&amf_ue_ngap_id) != 0) {
        ogs_error("Invalid AMF_UE_NGAP_ID");
        
        return;
    }

    target_ue = ran_ue_find_by_amf_ue_ngap_id_sps(&amf_ue_ngap_id);
    if (!target_ue) {
        ogs_error("No RAN UE Context : AMF_UE_NGAP_ID[%lld]",
                (long long)amf_ue_ngap_id);
        
        return;
    }

    source_ue = target_ue->source_ue;
    if (!source_ue) {
        ogs_error("Cannot find Source-UE Context [%lld]",
                (long long)amf_ue_ngap_id);
        
        return;
    }

    ogs_debug("    Source : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld] ",
        source_ue->ran_ue_ngap_id, (long long)source_ue->amf_ue_ngap_id);
    ogs_debug("    Target : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld] ",
        target_ue->ran_ue_ngap_id, (long long)target_ue->amf_ue_ngap_id);

    if (!Cause) {
        ogs_error("No Cause");
        
        return;
    }
    ogs_debug("    Cause[Group:%d Cause:%d]",
            Cause->present, (int)Cause->choice.radioNetwork);

    r = ngap_send_handover_preparation_failure(source_ue, Cause);
    ogs_expect(r == OGS_OK);
    ogs_assert(r != OGS_ERROR);

    r = ngap_send_ran_ue_context_release_command(target_ue,
            NGAP_Cause_PR_radioNetwork, NGAP_CauseRadioNetwork_ho_failure_in_target_5GC_ngran_node_or_target_system,
            NGAP_UE_CTX_REL_NG_HANDOVER_FAILURE, 0);
    ogs_expect(r == OGS_OK);
    ogs_assert(r != OGS_ERROR);
}

void ngap_handle_handover_cancel_sps(
        ran_ue_t *ran_ue, ogs_ngap_message_t *message)
{
    char buf[OGS_ADDRSTRLEN];
    int i,r;

    amf_ue_t *amf_ue = NULL;
    amf_sess_t *sess = NULL;
    ran_ue_t *source_ue = NULL, *target_ue = NULL;
    uint64_t amf_ue_ngap_id;
    amf_nsmf_pdusession_sm_context_param_t param;

    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_HandoverCancel_t *HandoverCancel = NULL;

    NGAP_HandoverCancelIEs_t *ie = NULL;
    NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
    NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
    NGAP_Cause_t *Cause = NULL;



    ogs_assert(message);
    initiatingMessage = message->choice.initiatingMessage;
    ogs_assert(initiatingMessage);
    HandoverCancel = &initiatingMessage->value.choice.HandoverCancel;
    ogs_assert(HandoverCancel);

    ogs_debug("HandoverCancel");

    for (i = 0; i < HandoverCancel->protocolIEs.list.count; i++) {
        ie = HandoverCancel->protocolIEs.list.array[i];
        switch (ie->id) {
        case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
            RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
            AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_Cause:
            Cause = &ie->value.choice.Cause;
            break;
        default:
            break;
        }
    }
   

    if (asn_INTEGER2ulong(AMF_UE_NGAP_ID,
                (unsigned long *)&amf_ue_ngap_id) != 0) {
        ogs_error("Invalid AMF_UE_NGAP_ID");
        
        return;
    }

    source_ue = ran_ue_find_by_amf_ue_ngap_id_sps(&amf_ue_ngap_id);
    if (!source_ue) {
        ogs_error("No RAN UE Context : AMF_UE_NGAP_ID[%lld]",
                (long long)amf_ue_ngap_id);
        
        return;
    }

    target_ue = source_ue->target_ue;
    if (!target_ue) {
        ogs_error("Cannot find Source-UE Context [%lld]",
                (long long)amf_ue_ngap_id);
        
        return;
    }
    amf_ue = source_ue->amf_ue;
    if (!amf_ue) {
        ogs_error("Cannot find AMF-UE Context [%lld]",
                (long long)amf_ue_ngap_id);
        
        return;
    }

    ogs_debug("    Source : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld] ",
        source_ue->ran_ue_ngap_id, (long long)source_ue->amf_ue_ngap_id);
    ogs_debug("    Target : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld] ",
        target_ue->ran_ue_ngap_id, (long long)target_ue->amf_ue_ngap_id);

    if (!Cause) {
        ogs_error("No Cause");
        r = ngap_send_error_indication_sps(
                source_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }
    ogs_debug("    Cause[Group:%d Cause:%d]",
            Cause->present, (int)Cause->choice.radioNetwork);

    ogs_list_for_each(&amf_ue->sess_list, sess) {
        memset(&param, 0, sizeof(param));
        param.hoState = OpenAPI_ho_state_CANCELLED;
        param.ngApCause.group = Cause->present;
        param.ngApCause.value = (int)Cause->choice.radioNetwork;

        r = amf_sess_sbi_discover_and_send(
                OGS_SBI_SERVICE_TYPE_NSMF_PDUSESSION, NULL,
                amf_nsmf_pdusession_build_update_sm_context,
                sess, AMF_UPDATE_SM_CONTEXT_HANDOVER_CANCEL, &param);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
    }
}
void   ngap_handle_handover_notification_sps(
        ran_ue_t *ran_ue, ogs_ngap_message_t *message)
{
    char buf[OGS_ADDRSTRLEN];
    int i,r;

    amf_ue_t *amf_ue = NULL;
    amf_sess_t *sess = NULL;
    ran_ue_t *source_ue = NULL, *target_ue = NULL;
    uint64_t amf_ue_ngap_id;
    amf_nsmf_pdusession_sm_context_param_t param;

    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_HandoverNotify_t *HandoverNotify = NULL;

    NGAP_HandoverNotifyIEs_t *ie = NULL;
    NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
    NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
    NGAP_UserLocationInformation_t *UserLocationInformation = NULL;
    NGAP_UserLocationInformationNR_t *UserLocationInformationNR = NULL;



    ogs_assert(message);
    initiatingMessage = message->choice.initiatingMessage;
    ogs_assert(initiatingMessage);
    HandoverNotify = &initiatingMessage->value.choice.HandoverNotify;
    ogs_assert(HandoverNotify);

    ogs_debug("HandoverNotify");

    for (i = 0; i < HandoverNotify->protocolIEs.list.count; i++) {
        ie = HandoverNotify->protocolIEs.list.array[i];
        switch (ie->id) {
        case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
            RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
            AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_UserLocationInformation:
            UserLocationInformation = &ie->value.choice.UserLocationInformation;
            break;
        default:
            break;
        }
    }

    if (asn_INTEGER2ulong(AMF_UE_NGAP_ID,
                 (unsigned long *)&amf_ue_ngap_id) != 0) {
         ogs_error("Invalid AMF_UE_NGAP_ID");
        
         return;
     }


    target_ue = ran_ue_find_by_amf_ue_ngap_id_sps(&amf_ue_ngap_id);
    if (!target_ue) {
        return;
    }

    source_ue = target_ue->source_ue;
    if (!source_ue) {
        return;
    }
    amf_ue = target_ue->amf_ue;
    if (!amf_ue) {
        ogs_error("Cannot find AMF-UE Context [%lld]",
                (long long)amf_ue_ngap_id);
        r = ngap_send_error_indication_sps(
                target_ue,
                NGAP_Cause_PR_radioNetwork,
                NGAP_CauseRadioNetwork_unknown_local_UE_NGAP_ID);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    amf_ue_associate_ran_ue(amf_ue, target_ue);

    if (!UserLocationInformation) {
        ogs_error("No UserLocationInformation");
        r = ngap_send_error_indication_sps(target_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    if (UserLocationInformation->present !=
            NGAP_UserLocationInformation_PR_userLocationInformationNR) {
        ogs_error("Not implemented UserLocationInformation[%d]",
                UserLocationInformation->present);
        r = ngap_send_error_indication_sps(target_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_unspecified);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    UserLocationInformationNR =
        UserLocationInformation->choice.userLocationInformationNR;
    ogs_assert(UserLocationInformationNR);
    ogs_ngap_ASN_to_nr_cgi(
            &UserLocationInformationNR->nR_CGI, &target_ue->saved.nr_cgi);
    ogs_ngap_ASN_to_5gs_tai(
            &UserLocationInformationNR->tAI, &target_ue->saved.nr_tai);

    ogs_debug("    Source : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld] ",
        source_ue->ran_ue_ngap_id, (long long)source_ue->amf_ue_ngap_id);
    ogs_debug("    Source : TAC[%d] CellID[0x%llx]",
        source_ue->saved.nr_tai.tac.v,
        (long long)source_ue->saved.nr_cgi.cell_id);
    ogs_debug("    Target : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld] ",
        target_ue->ran_ue_ngap_id, (long long)target_ue->amf_ue_ngap_id);
    ogs_debug("    Target : TAC[%d] CellID[0x%llx]",
        target_ue->saved.nr_tai.tac.v,
        (long long)target_ue->saved.nr_cgi.cell_id);

    /* Copy Stream-No/TAI/ECGI from ran_ue */
    amf_ue->gnb_ostream_id = target_ue->gnb_ostream_id;
    memcpy(&amf_ue->nr_tai, &target_ue->saved.nr_tai, sizeof(ogs_5gs_tai_t));
    memcpy(&amf_ue->nr_cgi, &target_ue->saved.nr_cgi, sizeof(ogs_nr_cgi_t));

    r = ngap_send_ran_ue_context_release_command(source_ue,
            NGAP_Cause_PR_radioNetwork,
            NGAP_CauseRadioNetwork_successful_handover,
            NGAP_UE_CTX_REL_NG_HANDOVER_COMPLETE,
            ogs_app()->time.handover.duration);
    ogs_expect(r == OGS_OK);
    ogs_assert(r != OGS_ERROR);

    ogs_list_for_each(&amf_ue->sess_list, sess) {
        memset(&param, 0, sizeof(param));
        param.hoState = OpenAPI_ho_state_COMPLETED;

        r = amf_sess_sbi_discover_and_send(
                OGS_SBI_SERVICE_TYPE_NSMF_PDUSESSION, NULL,
                amf_nsmf_pdusession_build_update_sm_context,
                sess, AMF_UPDATE_SM_CONTEXT_HANDOVER_NOTIFY, &param);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
    }
}       

 void ngap_handle_path_switch_request_sps(
        ran_ue_t *ran_ue,ogs_ngap_message_t *message)
{
    char buf[OGS_ADDRSTRLEN];
    int i,r;

    amf_ue_t *amf_ue = NULL;
    //ran_ue_t *ran_ue = NULL;
    //uint64_t amf_ue_ngap_id;

    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_PathSwitchRequest_t *PathSwitchRequest = NULL;

    NGAP_PathSwitchRequestIEs_t *ie = NULL;
    NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
    NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
    NGAP_UserLocationInformation_t *UserLocationInformation = NULL;
    NGAP_UserLocationInformationNR_t *UserLocationInformationNR = NULL;
    NGAP_UESecurityCapabilities_t *UESecurityCapabilities = NULL;
    NGAP_PDUSessionResourceToBeSwitchedDLList_t
        *PDUSessionResourceToBeSwitchedDLList = NULL;

    NGAP_NRencryptionAlgorithms_t *nRencryptionAlgorithms = NULL;
    NGAP_NRintegrityProtectionAlgorithms_t
        *nRintegrityProtectionAlgorithms = NULL;
    NGAP_EUTRAencryptionAlgorithms_t *eUTRAencryptionAlgorithms = NULL;
    NGAP_EUTRAintegrityProtectionAlgorithms_t
        *eUTRAintegrityProtectionAlgorithms = NULL;
    uint16_t nr_ea = 0, nr_ia = 0, eutra_ea = 0, eutra_ia = 0;
    uint8_t nr_ea0 = 0, nr_ia0 = 0, eutra_ea0 = 0, eutra_ia0 = 0;

    NGAP_PDUSessionResourceToBeSwitchedDLItem_t *PDUSessionItem = NULL;
    OCTET_STRING_t *transfer = NULL;

    amf_nsmf_pdusession_sm_context_param_t param;

   
    ogs_assert(message);
    initiatingMessage = message->choice.initiatingMessage;
    ogs_assert(initiatingMessage);
    PathSwitchRequest = &initiatingMessage->value.choice.PathSwitchRequest;
    ogs_assert(PathSwitchRequest);

    ogs_info("PathSwitchRequest");

    for (i = 0; i < PathSwitchRequest->protocolIEs.list.count; i++) {
        ie = PathSwitchRequest->protocolIEs.list.array[i];
        switch (ie->id) {
        case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
            RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_SourceAMF_UE_NGAP_ID:
            AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_UserLocationInformation:
            UserLocationInformation = &ie->value.choice.UserLocationInformation;
            break;
        case NGAP_ProtocolIE_ID_id_UESecurityCapabilities:
            UESecurityCapabilities = &ie->value.choice.UESecurityCapabilities;
            break;
        case NGAP_ProtocolIE_ID_id_PDUSessionResourceToBeSwitchedDLList:
            PDUSessionResourceToBeSwitchedDLList =
                &ie->value.choice.PDUSessionResourceToBeSwitchedDLList;
            break;
        default:
            break;
        }
    }

    amf_ue = ran_ue->amf_ue;
    if (!amf_ue) {
        ogs_error("Cannot find AMF-UE Context [%lld]",
                (long long)ran_ue->amf_ue_ngap_id);
        r = ngap_send_error_indication_sps(
                ran_ue,
                NGAP_Cause_PR_radioNetwork,
                NGAP_CauseRadioNetwork_unknown_local_UE_NGAP_ID);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    ogs_info("    [OLD] RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld] ",
        ran_ue->ran_ue_ngap_id, (long long)ran_ue->amf_ue_ngap_id);
    ogs_info("    [OLD] TAC[%d] CellID[0x%llx]",
        amf_ue->nr_tai.tac.v, (long long)amf_ue->nr_cgi.cell_id);

    /* Update RAN-UE-NGAP-ID */
    ran_ue->ran_ue_ngap_id = *RAN_UE_NGAP_ID;

    /* Change ran_ue to the NEW gNB */
    //ran_ue_switch_to_gnb(ran_ue, gnb);

    if (!UserLocationInformation) {
        ogs_error("No UserLocationInformation");
        r = ngap_send_error_indication2_sps(amf_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    if (UserLocationInformation->present !=
            NGAP_UserLocationInformation_PR_userLocationInformationNR) {
        ogs_error("Not implemented UserLocationInformation[%d]",
                UserLocationInformation->present);
        r = ngap_send_error_indication2_sps(amf_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_unspecified);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    if (!UESecurityCapabilities) {
        ogs_error("No UESecurityCapabilities");
        r = ngap_send_error_indication2_sps(amf_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    if (!PDUSessionResourceToBeSwitchedDLList) {
        ogs_error("No PDUSessionResourceToBeSwitchedDLList");
        r = ngap_send_error_indication2_sps(amf_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    if (!SECURITY_CONTEXT_IS_VALID(amf_ue)) {
        ogs_error("No Security Context");
        r = ngap_send_error_indication2_sps(amf_ue,
                NGAP_Cause_PR_nas, NGAP_CauseNas_authentication_failure);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    ogs_info("    [NEW] RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld] ",
        ran_ue->ran_ue_ngap_id, (long long)ran_ue->amf_ue_ngap_id);

    UserLocationInformationNR =
            UserLocationInformation->choice.userLocationInformationNR;
    ogs_assert(UserLocationInformationNR);
    ogs_ngap_ASN_to_nr_cgi(
            &UserLocationInformationNR->nR_CGI, &ran_ue->saved.nr_cgi);
    ogs_ngap_ASN_to_5gs_tai(
            &UserLocationInformationNR->tAI, &ran_ue->saved.nr_tai);

    /* Copy Stream-No/TAI/ECGI from ran_ue */
    amf_ue->gnb_ostream_id = ran_ue->gnb_ostream_id;
    memcpy(&amf_ue->nr_tai, &ran_ue->saved.nr_tai, sizeof(ogs_5gs_tai_t));
    memcpy(&amf_ue->nr_cgi, &ran_ue->saved.nr_cgi, sizeof(ogs_nr_cgi_t));

    ogs_info("    [NEW] TAC[%d] CellID[0x%llx]",
        amf_ue->nr_tai.tac.v, (long long)amf_ue->nr_cgi.cell_id);

    nRencryptionAlgorithms = &UESecurityCapabilities->nRencryptionAlgorithms;
    nRintegrityProtectionAlgorithms =
        &UESecurityCapabilities->nRintegrityProtectionAlgorithms;
    eUTRAencryptionAlgorithms =
        &UESecurityCapabilities->eUTRAencryptionAlgorithms;
    eUTRAintegrityProtectionAlgorithms =
        &UESecurityCapabilities->eUTRAintegrityProtectionAlgorithms;

    memcpy(&nr_ea, nRencryptionAlgorithms->buf, sizeof(nr_ea));
    nr_ea = be16toh(nr_ea);
    nr_ea0 = amf_ue->ue_security_capability.nr_ea0;
    amf_ue->ue_security_capability.nr_ea = nr_ea >> 9;
    amf_ue->ue_security_capability.nr_ea0 = nr_ea0;

    memcpy(&nr_ia, nRintegrityProtectionAlgorithms->buf, sizeof(nr_ia));
    nr_ia = be16toh(nr_ia);
    nr_ia0 = amf_ue->ue_security_capability.nr_ia0;
    amf_ue->ue_security_capability.nr_ia = nr_ia >> 9;
    amf_ue->ue_security_capability.nr_ia0 = nr_ia0;

    memcpy(&eutra_ea, eUTRAencryptionAlgorithms->buf, sizeof(eutra_ea));
    eutra_ea = be16toh(eutra_ea);
    eutra_ea0 = amf_ue->ue_security_capability.eutra_ea0;
    amf_ue->ue_security_capability.eutra_ea = eutra_ea >> 9;
    amf_ue->ue_security_capability.eutra_ea0 = eutra_ea0;

    memcpy(&eutra_ia,
            eUTRAintegrityProtectionAlgorithms->buf, sizeof(eutra_ia));
    eutra_ia = be16toh(eutra_ia);
    eutra_ia0 = amf_ue->ue_security_capability.eutra_ia0;
    amf_ue->ue_security_capability.eutra_ia = eutra_ia >> 9;
    amf_ue->ue_security_capability.eutra_ia0 = eutra_ia0;

    /* Update Security Context (NextHop) */
    amf_ue->nhcc++;
    ogs_kdf_nh_gnb(amf_ue->kamf, amf_ue->nh, amf_ue->nh);

    for (i = 0; i < PDUSessionResourceToBeSwitchedDLList->list.count; i++) {
        amf_sess_t *sess = NULL;
        PDUSessionItem = (NGAP_PDUSessionResourceToBeSwitchedDLItem_t *)
            PDUSessionResourceToBeSwitchedDLList->list.array[i];

        if (!PDUSessionItem) {
            ogs_error("No NGAP_PDUSessionResourceToBeSwitchedDLItem");
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        transfer = &PDUSessionItem->pathSwitchRequestTransfer;
        if (!transfer) {
            ogs_error("No PDUSessionResourceSetupResponseTransfer");
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        if (PDUSessionItem->pDUSessionID ==
                OGS_NAS_PDU_SESSION_IDENTITY_UNASSIGNED) {
            ogs_error("PDU Session Identity is unassigned");
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        sess = amf_sess_find_by_psi(amf_ue, PDUSessionItem->pDUSessionID);
        if (!sess) {
            ogs_error("Cannot find PDU Session ID [%d]",
                    (int)PDUSessionItem->pDUSessionID);
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_radioNetwork,
                    NGAP_CauseRadioNetwork_unknown_PDU_session_ID);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        if (!SESSION_CONTEXT_IN_SMF(sess)) {
            ogs_error("Session Context is not in SMF [%d]",
                    (int)PDUSessionItem->pDUSessionID);
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_radioNetwork,
                    NGAP_CauseRadioNetwork_unknown_PDU_session_ID);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        memset(&param, 0, sizeof(param));
        param.n2smbuf = ogs_pkbuf_alloc(NULL, OGS_MAX_SDU_LEN);
        ogs_assert(param.n2smbuf);
        param.n2SmInfoType = OpenAPI_n2_sm_info_type_PATH_SWITCH_REQ;
        ogs_pkbuf_put_data(param.n2smbuf, transfer->buf, transfer->size);

        r = amf_sess_sbi_discover_and_send(
                OGS_SBI_SERVICE_TYPE_NSMF_PDUSESSION, NULL,
                amf_nsmf_pdusession_build_update_sm_context,
                sess, AMF_UPDATE_SM_CONTEXT_PATH_SWITCH_REQUEST, &param);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);

        ogs_pkbuf_free(param.n2smbuf);
    }
}
void ngap_handle_pdu_session_resource_modify_response_sps(
        ran_ue_t *ran_ue, ogs_ngap_message_t *message)
{
    char buf[OGS_ADDRSTRLEN];
    int i,r;

    amf_ue_t *amf_ue = NULL;
    //ran_ue_t *ran_ue = NULL;
    //uint64_t amf_ue_ngap_id;
    amf_nsmf_pdusession_sm_context_param_t param;

    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    NGAP_PDUSessionResourceModifyResponse_t *PDUSessionResourceModifyResponse;

    NGAP_PDUSessionResourceModifyResponseIEs_t *ie = NULL;
    NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
    NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
    NGAP_PDUSessionResourceModifyListModRes_t *PDUSessionList = NULL;
    NGAP_PDUSessionResourceModifyItemModRes_t *PDUSessionItem = NULL;
    OCTET_STRING_t *transfer = NULL;


    ogs_assert(message);
    successfulOutcome = message->choice.successfulOutcome;
    ogs_assert(successfulOutcome);
    PDUSessionResourceModifyResponse =
        &successfulOutcome->value.choice.PDUSessionResourceModifyResponse;
    ogs_assert(PDUSessionResourceModifyResponse);

    ogs_debug("PDUSessionResourceModifyResponse");

    for (i = 0; i < PDUSessionResourceModifyResponse->protocolIEs.list.count;
            i++) {
        ie = PDUSessionResourceModifyResponse->protocolIEs.list.array[i];
        switch (ie->id) {
        case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
            RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
            AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_PDUSessionResourceModifyListModRes:
            PDUSessionList =
                &ie->value.choice.PDUSessionResourceModifyListModRes;
            break;
        default:
            break;
        }
    }
    amf_ue = ran_ue->amf_ue;
    if (!amf_ue) {
        ogs_error("Cannot find AMF-UE Context");
        r =  ngap_send_error_indication_sps(
                ran_ue,
                NGAP_Cause_PR_radioNetwork,
                NGAP_CauseRadioNetwork_unknown_local_UE_NGAP_ID);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    if (!PDUSessionList) {
        ogs_error("No PDUSessionResourceModifyListModRes");
        r = ngap_send_error_indication2_sps(amf_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    for (i = 0; i < PDUSessionList->list.count; i++) {
        amf_sess_t *sess = NULL;
        PDUSessionItem = (NGAP_PDUSessionResourceModifyItemModRes_t *)
            PDUSessionList->list.array[i];

        if (!PDUSessionItem) {
            ogs_error("No PDUSessionResourceModifyItemModRes");
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        transfer = &PDUSessionItem->pDUSessionResourceModifyResponseTransfer;
        if (!transfer) {
            ogs_error("No PDUSessionResourceModifyResponseTransfer");
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        if (PDUSessionItem->pDUSessionID ==
                OGS_NAS_PDU_SESSION_IDENTITY_UNASSIGNED) {
            ogs_error("PDU Session Identity is unassigned");
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        sess = amf_sess_find_by_psi(amf_ue, PDUSessionItem->pDUSessionID);
        if (!sess) {
            ogs_error("Cannot find PDU Session ID [%d]",
                    (int)PDUSessionItem->pDUSessionID);
            r =     ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_radioNetwork,
                    NGAP_CauseRadioNetwork_unknown_PDU_session_ID);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        if (!SESSION_CONTEXT_IN_SMF(sess)) {
            ogs_error("Session Context is not in SMF [%d]",
                    (int)PDUSessionItem->pDUSessionID);
            r = ngap_send_error_indication2_sps(amf_ue,
                    NGAP_Cause_PR_radioNetwork,
                    NGAP_CauseRadioNetwork_unknown_PDU_session_ID);
            ogs_expect(r == OGS_OK);
            ogs_assert(r != OGS_ERROR);
            return;
        }

        memset(&param, 0, sizeof(param));
        param.n2smbuf = ogs_pkbuf_alloc(NULL, OGS_MAX_SDU_LEN);
        ogs_assert(param.n2smbuf);
        param.n2SmInfoType = OpenAPI_n2_sm_info_type_PDU_RES_MOD_RSP;
        ogs_pkbuf_put_data(param.n2smbuf, transfer->buf, transfer->size);

        r = amf_sess_sbi_discover_and_send(
                OGS_SBI_SERVICE_TYPE_NSMF_PDUSESSION, NULL,
                amf_nsmf_pdusession_build_update_sm_context,
                sess, AMF_UPDATE_SM_CONTEXT_MODIFIED, &param);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);

        ogs_pkbuf_free(param.n2smbuf);
    }
}

void ngap_handle_ue_context_release_complete_sps(
                ran_ue_t *ran_ue,ogs_ngap_message_t *message)
{
    int i,r;
    char buf[OGS_ADDRSTRLEN];
    uint64_t amf_ue_ngap_id;     
    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    NGAP_UEContextReleaseComplete_t *UEContextReleaseComplete = NULL;
        
    NGAP_UEContextReleaseComplete_IEs_t *ie = NULL;
    NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
    NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
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
        r = ngap_send_error_indication_sps(ran_ue,
                        NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
   }
        
    if (asn_INTEGER2ulong(AMF_UE_NGAP_ID,
                (unsigned long *)&amf_ue_ngap_id) != 0) {
        ogs_error("Invalid AMF_UE_NGAP_ID");
        r = ngap_send_error_indication_sps(ran_ue,
                NGAP_Cause_PR_protocol, NGAP_CauseProtocol_semantic_error);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    ran_ue = ran_ue_find_by_amf_ue_ngap_id_sps(&amf_ue_ngap_id);
    if (!ran_ue) {
        ogs_error("No RAN UE Context : AMF_UE_NGAP_ID[%lld]",
                (long long)amf_ue_ngap_id);
        r = ngap_send_error_indication_sps(
                ran_ue,
                NGAP_Cause_PR_radioNetwork,
                NGAP_CauseRadioNetwork_unknown_local_UE_NGAP_ID);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }
  
    ngap_handle_ue_context_release_action(ran_ue);
           
           
}
void ngap_handle_ng_reset_sps(        ogs_ngap_message_t *message)
{
    char buf[OGS_ADDRSTRLEN];
    int i, old_xact_count = 0, new_xact_count = 0;


    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_NGReset_t *NGReset = NULL;

    NGAP_NGResetIEs_t *ie = NULL;
    NGAP_Cause_t *Cause = NULL;
    NGAP_ResetType_t *ResetType = NULL;
    NGAP_UE_associatedLogicalNG_connectionList_t *partOfNG_Interface = NULL;

    ran_ue_t *iter = NULL;


    ogs_assert(message);
    initiatingMessage = message->choice.initiatingMessage;
    ogs_assert(initiatingMessage);
    NGReset = &initiatingMessage->value.choice.NGReset;
    ogs_assert(NGReset);

    ogs_warn("NGReset");

    for (i = 0; i < NGReset->protocolIEs.list.count; i++) {
        ie = NGReset->protocolIEs.list.array[i];
        switch (ie->id) {
        case NGAP_ProtocolIE_ID_id_Cause:
            Cause = &ie->value.choice.Cause;
            break;
        case NGAP_ProtocolIE_ID_id_ResetType:
            ResetType = &ie->value.choice.ResetType;
            break;
        default:
            break;
        }
    }

    if (!Cause) {
        ogs_error("No Cause");       
        return;
    }

    ogs_warn("    Cause[Group:%d Cause:%d]",
            Cause->present, (int)Cause->choice.radioNetwork);

    if (!ResetType) {
        ogs_error("No ResetType");
        return;
    }

    switch (ResetType->present) {
    case NGAP_ResetType_PR_partOfNG_Interface:
        ogs_warn("    NGAP_ResetType_PR_partOfNG_Interface");

        partOfNG_Interface = ResetType->choice.partOfNG_Interface;
        ogs_assert(partOfNG_Interface);

        for (i = 0; i < partOfNG_Interface->list.count; i++) {
            NGAP_UE_associatedLogicalNG_connectionItem_t *item = NULL;
            uint64_t amf_ue_ngap_id;

            ran_ue_t *ran_ue = NULL;
            amf_ue_t *amf_ue = NULL;

            item = (NGAP_UE_associatedLogicalNG_connectionItem_t *)
                        partOfNG_Interface->list.array[i];
            if (!item) {
                ogs_error("No ResetType");
                continue;
            }

            if (item->aMF_UE_NGAP_ID) {
                if (asn_INTEGER2ulong(item->aMF_UE_NGAP_ID,
                            (unsigned long *)&amf_ue_ngap_id) != 0) {
                    ogs_error("Invalid AMF_UE_NGAP_ID");
                    continue;
                }

                ran_ue = ran_ue_find_by_amf_ue_ngap_id_sps(&amf_ue_ngap_id);

                if (!ran_ue) {
                    ogs_error("No RAN UE Context : AMF_UE_NGAP_ID[%lld]",
                            (long long)amf_ue_ngap_id);
                    continue;
                }

            } 

            ogs_assert(ran_ue);

            /* RAN_UE Context where PartOfNG_interface was requested */
            ran_ue->part_of_ng_reset_requested = true;

            amf_ue = ran_ue->amf_ue;
            ogs_assert(amf_ue);

            old_xact_count = amf_sess_xact_count(amf_ue);

            amf_sbi_send_deactivate_all_sessions(
                amf_ue, AMF_REMOVE_S1_CONTEXT_BY_RESET_PARTIAL,
                NGAP_Cause_PR_radioNetwork,
                NGAP_CauseRadioNetwork_failure_in_radio_interface_procedure);

            new_xact_count = amf_sess_xact_count(amf_ue);
            ogs_info("????????????????????   old_xact_count =%d,new_xact_count=%d",old_xact_count ,new_xact_count);
            if (old_xact_count == new_xact_count) ran_ue_remove_sps(ran_ue);
        }
#if 0
        ogs_list_for_each(&gnb->ran_ue_list, iter) {
            if (iter->part_of_ng_reset_requested == true) {
                /* The GNB_UE context
                 * where PartOfNG_interface was requested
                 * still remains */
                return;
            }
        }

        /* All GNB_UE context
         * where PartOfNG_interface was requested
         * REMOVED */
        ogs_assert(gnb->ng_reset_ack);
        ogs_expect(OGS_OK ==
            ngap_send_to_gnb(gnb, gnb->ng_reset_ack, NGAP_NON_UE_SIGNALLING));

        /* Clear NG-Reset Ack Buffer */
        gnb->ng_reset_ack = NULL;
#endif
        break;
    default:
        ogs_warn("Invalid ResetType[%d]", ResetType->present);
        break;
    }
}

void ngap_handle_uplink_nas_transport_sps(
        ran_ue_t *ran_ue,ogs_ngap_message_t *message)
{
    char buf[OGS_ADDRSTRLEN];
    int i, r;

    amf_ue_t *amf_ue = NULL;
    
    uint64_t amf_ue_ngap_id;

    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_UplinkNASTransport_t *UplinkNASTransport = NULL;

    NGAP_UplinkNASTransport_IEs_t *ie = NULL;
    NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
    NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
    NGAP_NAS_PDU_t *NAS_PDU = NULL;
    NGAP_UserLocationInformation_t *UserLocationInformation = NULL;
    NGAP_UserLocationInformationNR_t *UserLocationInformationNR = NULL;

    

    ogs_assert(message);
    initiatingMessage = message->choice.initiatingMessage;
    ogs_assert(initiatingMessage);
    UplinkNASTransport = &initiatingMessage->value.choice.UplinkNASTransport;
    ogs_assert(UplinkNASTransport);

    ogs_debug("UplinkNASTransport");

    for (i = 0; i < UplinkNASTransport->protocolIEs.list.count; i++) {
        ie = UplinkNASTransport->protocolIEs.list.array[i];
        switch (ie->id) {
        case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
            RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
            AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
            break;
        case NGAP_ProtocolIE_ID_id_NAS_PDU:
            NAS_PDU = &ie->value.choice.NAS_PDU;
            break;
        case NGAP_ProtocolIE_ID_id_UserLocationInformation:
            UserLocationInformation = &ie->value.choice.UserLocationInformation;
            break;
        default:
            break;
        }
    }


    if (!AMF_UE_NGAP_ID) {
        ogs_error("No AMF_UE_NGAP_ID");
        return;
    }

    if (asn_INTEGER2ulong(AMF_UE_NGAP_ID,
                (unsigned long *)&amf_ue_ngap_id) != 0) {
        ogs_error("Invalid AMF_UE_NGAP_ID");

        return;
    }   

    amf_ue = ran_ue->amf_ue;
    if (!amf_ue) {
        ogs_error("Cannot find AMF-UE Context [%lld]",
                (long long)amf_ue_ngap_id);
        r = ngap_send_error_indication_sps(
                ran_ue,
                NGAP_Cause_PR_radioNetwork,
                NGAP_CauseRadioNetwork_unknown_local_UE_NGAP_ID);
        ogs_expect(r == OGS_OK);
        ogs_assert(r != OGS_ERROR);
        return;
    }

    if (!UserLocationInformation) {
        ogs_error("No UserLocationInformation");       
        return;
    }

    if (UserLocationInformation->present !=
            NGAP_UserLocationInformation_PR_userLocationInformationNR) {
        ogs_error("Not implemented UserLocationInformation[%d]",
                UserLocationInformation->present);
        return;
    }

    if (!NAS_PDU) {
        ogs_error("No NAS_PDU");
        return;
    }

    UserLocationInformationNR =
        UserLocationInformation->choice.userLocationInformationNR;
    ogs_assert(UserLocationInformationNR);
    ogs_ngap_ASN_to_nr_cgi(
            &UserLocationInformationNR->nR_CGI, &ran_ue->saved.nr_cgi);
    ogs_ngap_ASN_to_5gs_tai(
            &UserLocationInformationNR->tAI, &ran_ue->saved.nr_tai);

    ogs_debug("    RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%lld] "
            "TAC[%d] CellID[0x%llx]",
        ran_ue->ran_ue_ngap_id, (long long)ran_ue->amf_ue_ngap_id,
        ran_ue->saved.nr_tai.tac.v, (long long)ran_ue->saved.nr_cgi.cell_id);

    /* Copy NR-TAI/NR-CGI from ran_ue */
    memcpy(&amf_ue->nr_tai, &ran_ue->saved.nr_tai, sizeof(ogs_5gs_tai_t));
    memcpy(&amf_ue->nr_cgi, &ran_ue->saved.nr_cgi, sizeof(ogs_nr_cgi_t));

    ngap_send_to_nas(ran_ue, NGAP_ProcedureCode_id_UplinkNASTransport, NAS_PDU);
}
void sps_check_icps_offline(void)
{
    if(send_heart_cnt>2){
        ogs_warn("send_heart_cnt:%d",send_heart_cnt);
        if(send_heart_cnt>4){
            ran_ue_remove_all();
            send_heart_cnt=0;
        }
    }
}
