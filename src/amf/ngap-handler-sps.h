#ifndef AMF_NGAP_HANDLER_SPS_H
#define AMF_NGAP_HANDLER_SPS_H

#include "context.h"




#ifdef __cplusplus
extern "C" {
#endif
typedef struct NGAP_icps_send_head_s{	
	NGAP_UEContextRequest_t UEContextRequest;	
	NGAP_ProcedureCode_t ProcedureCode;
	size_t size;
	uint8_t PDUsessioncount;
	uint8_t PDUsessionfailedcount;
	
}NGAP_icps_send_head_t;


typedef struct PDUSessionResource_s{
	
	NGAP_PDUSessionID_t	 pDUSessionID;
	size_t size;
	uint8_t *buf;
}PDUSessionResource_t;
	

typedef struct UERadioCapability_s{
	size_t size;	
	uint8_t *buf;
}UERadioCapability_t;

typedef struct NGAP_Cause_sps_s{
	NGAP_Cause_PR present;
	union NGAP_Cause_sps_u {
		NGAP_CauseRadioNetwork_t	 radioNetwork;
		NGAP_CauseTransport_t	 transport;
		NGAP_CauseNas_t	 nas;
		NGAP_CauseProtocol_t	 protocol;
		NGAP_CauseMisc_t	 misc;		
	} choice;
}NGAP_Cause_sps_t;


int sps_handle_rev_ini_ngap(amf_internel_msg_header_t *pmsg,ogs_pkbuf_t *pkbuf);//O3
void ngap_handle_initial_context_setup_response_sps(ran_ue_t *ran_ue,uint8_t count,size_t size,uint8_t *buf);
void ngap_handle_ue_radio_capability_info_indication_sps(ran_ue_t *ran_ue,size_t size,uint8_t *buf);
void ngap_handle_ue_context_release_request_sps(ran_ue_t *ran_ue,uint8_t count,size_t size,uint8_t *buf);
void ngap_handle_pdu_session_resource_setup_response_sps(ran_ue_t *ran_ue,ogs_ngap_message_t *message);
void print_buf(unsigned char *buf, int len);
void ngap_handle_pdu_session_resource_release_response_sps(ran_ue_t *ran_ue,ogs_ngap_message_t *message);
void ngap_handle_ue_context_release_complete_sps(ran_ue_t *ran_ue,ogs_ngap_message_t *message);


#ifdef __cplusplus
}
#endif

#endif /* AMF_NGAP_HANDLER_SPS_H */


