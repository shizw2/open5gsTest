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

int sps_handle_rev_ini_ngap(amf_internel_msg_header_t *pmsg,ogs_pkbuf_t *pkbuf);//O3
void ngap_handle_initial_context_setup_response_sps(ran_ue_t * ran_ue,uint8_t count,size_t size,uint8_t *buf);
void ngap_handle_ue_radio_capability_info_indication_sps(ran_ue_t * ran_ue,size_t size,uint8_t *buf);





#ifdef __cplusplus
}
#endif

#endif /* AMF_NGAP_HANDLER_SPS_H */


